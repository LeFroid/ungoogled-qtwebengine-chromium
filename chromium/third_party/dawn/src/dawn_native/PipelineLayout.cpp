// Copyright 2017 The Dawn Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "dawn_native/PipelineLayout.h"

#include "common/Assert.h"
#include "common/BitSetIterator.h"
#include "common/HashUtils.h"
#include "common/ityp_stack_vec.h"
#include "dawn_native/BindGroupLayout.h"
#include "dawn_native/Device.h"
#include "dawn_native/ShaderModule.h"

namespace dawn_native {

    MaybeError ValidatePipelineLayoutDescriptor(DeviceBase* device,
                                                const PipelineLayoutDescriptor* descriptor) {
        if (descriptor->nextInChain != nullptr) {
            return DAWN_VALIDATION_ERROR("nextInChain must be nullptr");
        }

        if (descriptor->bindGroupLayoutCount > kMaxBindGroups) {
            return DAWN_VALIDATION_ERROR("too many bind group layouts");
        }

        BindingCounts bindingCounts = {};
        for (uint32_t i = 0; i < descriptor->bindGroupLayoutCount; ++i) {
            DAWN_TRY(device->ValidateObject(descriptor->bindGroupLayouts[i]));
            AccumulateBindingCounts(&bindingCounts,
                                    descriptor->bindGroupLayouts[i]->GetBindingCountInfo());
        }

        DAWN_TRY(ValidateBindingCounts(bindingCounts));
        return {};
    }

    // PipelineLayoutBase

    PipelineLayoutBase::PipelineLayoutBase(DeviceBase* device,
                                           const PipelineLayoutDescriptor* descriptor)
        : CachedObject(device) {
        ASSERT(descriptor->bindGroupLayoutCount <= kMaxBindGroups);
        for (BindGroupIndex group(0); group < BindGroupIndex(descriptor->bindGroupLayoutCount);
             ++group) {
            mBindGroupLayouts[group] = descriptor->bindGroupLayouts[static_cast<uint32_t>(group)];
            mMask.set(group);
        }
    }

    PipelineLayoutBase::PipelineLayoutBase(DeviceBase* device, ObjectBase::ErrorTag tag)
        : CachedObject(device, tag) {
    }

    PipelineLayoutBase::~PipelineLayoutBase() {
        // Do not uncache the actual cached object if we are a blueprint
        if (IsCachedReference()) {
            GetDevice()->UncachePipelineLayout(this);
        }
    }

    // static
    PipelineLayoutBase* PipelineLayoutBase::MakeError(DeviceBase* device) {
        return new PipelineLayoutBase(device, ObjectBase::kError);
    }

    // static
    ResultOrError<PipelineLayoutBase*> PipelineLayoutBase::CreateDefault(
        DeviceBase* device,
        std::vector<StageAndDescriptor> stages) {
        using EntryMap = std::map<BindingNumber, BindGroupLayoutEntry>;

        // Merges two entries at the same location, if they are allowed to be merged.
        auto MergeEntries = [](BindGroupLayoutEntry* modifiedEntry,
                               const BindGroupLayoutEntry& mergedEntry) -> MaybeError {
            // Minimum buffer binding size excluded because we take the maximum seen across stages.
            // Visibility is excluded because we take the OR across stages.
            bool compatible =
                modifiedEntry->binding == mergedEntry.binding &&                    //
                modifiedEntry->type == mergedEntry.type &&                          //
                modifiedEntry->hasDynamicOffset == mergedEntry.hasDynamicOffset &&  //
                modifiedEntry->viewDimension == mergedEntry.viewDimension &&        //
                modifiedEntry->textureComponentType == mergedEntry.textureComponentType;

            // Check if any properties are incompatible with existing entry
            // If compatible, we will merge some properties
            if (!compatible) {
                return DAWN_VALIDATION_ERROR(
                    "Duplicate binding in default pipeline layout initialization "
                    "not compatible with previous declaration");
            }

            // Use the max |minBufferBindingSize| we find.
            modifiedEntry->minBufferBindingSize =
                std::max(modifiedEntry->minBufferBindingSize, mergedEntry.minBufferBindingSize);

            // Use the OR of all the stages at which we find this binding.
            modifiedEntry->visibility |= mergedEntry.visibility;

            return {};
        };

        // Does the trivial conversions from a ShaderBindingInfo to a BindGroupLayoutEntry
        auto ConvertMetadataToEntry =
            [](const EntryPointMetadata::ShaderBindingInfo& shaderBinding) -> BindGroupLayoutEntry {
            BindGroupLayoutEntry entry = {};
            entry.type = shaderBinding.type;
            entry.hasDynamicOffset = false;
            entry.viewDimension = shaderBinding.viewDimension;
            entry.textureComponentType =
                Format::FormatTypeToTextureComponentType(shaderBinding.textureComponentType);
            entry.storageTextureFormat = shaderBinding.storageTextureFormat;
            entry.minBufferBindingSize = shaderBinding.minBufferBindingSize;
            return entry;
        };

        // Creates the BGL from the entries for a stage, checking it is valid.
        auto CreateBGL = [](DeviceBase* device,
                            const EntryMap& entries) -> ResultOrError<Ref<BindGroupLayoutBase>> {
            std::vector<BindGroupLayoutEntry> entryVec;
            entryVec.reserve(entries.size());
            for (auto& it : entries) {
                entryVec.push_back(it.second);
            }

            BindGroupLayoutDescriptor desc = {};
            desc.entries = entryVec.data();
            desc.entryCount = entryVec.size();

            DAWN_TRY(ValidateBindGroupLayoutDescriptor(device, &desc));
            return device->GetOrCreateBindGroupLayout(&desc);
        };

        ASSERT(!stages.empty());

        // Data which BindGroupLayoutDescriptor will point to for creation
        ityp::array<BindGroupIndex, std::map<BindingNumber, BindGroupLayoutEntry>, kMaxBindGroups>
            entryData = {};

        // Loops over all the reflected BindGroupLayoutEntries from shaders.
        for (const StageAndDescriptor& stage : stages) {
            SingleShaderStage shaderStage = stage.first;
            const EntryPointMetadata::BindingInfo& info =
                stage.second->module->GetEntryPoint(stage.second->entryPoint, shaderStage).bindings;

            for (BindGroupIndex group(0); group < info.size(); ++group) {
                for (const auto& bindingIt : info[group]) {
                    BindingNumber bindingNumber = bindingIt.first;
                    const EntryPointMetadata::ShaderBindingInfo& shaderBinding = bindingIt.second;

                    // Create the BindGroupLayoutEntry
                    BindGroupLayoutEntry entry = ConvertMetadataToEntry(shaderBinding);
                    entry.binding = static_cast<uint32_t>(bindingNumber);
                    entry.visibility = StageBit(shaderStage);

                    // Add it to our map of all entries, if there is an existing entry, then we
                    // need to merge, if we can.
                    const auto& insertion = entryData[group].insert({bindingNumber, entry});
                    if (!insertion.second) {
                        DAWN_TRY(MergeEntries(&insertion.first->second, entry));
                    }
                }
            }
        }

        // Create the bind group layouts. We need to keep track of the last non-empty BGL because
        // Dawn doesn't yet know that an empty BGL and a null BGL are the same thing.
        // TODO(cwallez@ch40m1um.qjz9zk): remove this when Dawn knows that empty and null BGL are the
        // same.
        BindGroupIndex pipelineBGLCount = BindGroupIndex(0);
        ityp::array<BindGroupIndex, Ref<BindGroupLayoutBase>, kMaxBindGroups> bindGroupLayouts = {};
        for (BindGroupIndex group(0); group < kMaxBindGroupsTyped; ++group) {
            DAWN_TRY_ASSIGN(bindGroupLayouts[group], CreateBGL(device, entryData[group]));
            if (entryData[group].size() != 0) {
                pipelineBGLCount = group + BindGroupIndex(1);
            }
        }

        // Create the deduced pipeline layout, validating if it is valid.
        PipelineLayoutBase* pipelineLayout = nullptr;
        {
            ityp::array<BindGroupIndex, BindGroupLayoutBase*, kMaxBindGroups> bgls = {};
            for (BindGroupIndex group(0); group < pipelineBGLCount; ++group) {
                bgls[group] = bindGroupLayouts[group].Get();
            }

            PipelineLayoutDescriptor desc = {};
            desc.bindGroupLayouts = bgls.data();
            desc.bindGroupLayoutCount = static_cast<uint32_t>(pipelineBGLCount);

            DAWN_TRY(ValidatePipelineLayoutDescriptor(device, &desc));
            DAWN_TRY_ASSIGN(pipelineLayout, device->GetOrCreatePipelineLayout(&desc));

            ASSERT(!pipelineLayout->IsError());
        }

        // Sanity check in debug that the pipeline layout is compatible with the current pipeline.
        for (const StageAndDescriptor& stage : stages) {
            const EntryPointMetadata& metadata =
                stage.second->module->GetEntryPoint(stage.second->entryPoint, stage.first);
            ASSERT(ValidateCompatibilityWithPipelineLayout(metadata, pipelineLayout).IsSuccess());
        }

        return pipelineLayout;
    }

    const BindGroupLayoutBase* PipelineLayoutBase::GetBindGroupLayout(BindGroupIndex group) const {
        ASSERT(!IsError());
        ASSERT(group < kMaxBindGroupsTyped);
        ASSERT(mMask[group]);
        const BindGroupLayoutBase* bgl = mBindGroupLayouts[group].Get();
        ASSERT(bgl != nullptr);
        return bgl;
    }

    BindGroupLayoutBase* PipelineLayoutBase::GetBindGroupLayout(BindGroupIndex group) {
        ASSERT(!IsError());
        ASSERT(group < kMaxBindGroupsTyped);
        ASSERT(mMask[group]);
        BindGroupLayoutBase* bgl = mBindGroupLayouts[group].Get();
        ASSERT(bgl != nullptr);
        return bgl;
    }

    const BindGroupLayoutMask& PipelineLayoutBase::GetBindGroupLayoutsMask() const {
        ASSERT(!IsError());
        return mMask;
    }

    BindGroupLayoutMask PipelineLayoutBase::InheritedGroupsMask(
        const PipelineLayoutBase* other) const {
        ASSERT(!IsError());
        return {(1 << static_cast<uint32_t>(GroupsInheritUpTo(other))) - 1u};
    }

    BindGroupIndex PipelineLayoutBase::GroupsInheritUpTo(const PipelineLayoutBase* other) const {
        ASSERT(!IsError());

        for (BindGroupIndex i(0); i < kMaxBindGroupsTyped; ++i) {
            if (!mMask[i] || mBindGroupLayouts[i].Get() != other->mBindGroupLayouts[i].Get()) {
                return i;
            }
        }
        return kMaxBindGroupsTyped;
    }

    size_t PipelineLayoutBase::HashFunc::operator()(const PipelineLayoutBase* pl) const {
        size_t hash = Hash(pl->mMask);

        for (BindGroupIndex group : IterateBitSet(pl->mMask)) {
            HashCombine(&hash, pl->GetBindGroupLayout(group));
        }

        return hash;
    }

    bool PipelineLayoutBase::EqualityFunc::operator()(const PipelineLayoutBase* a,
                                                      const PipelineLayoutBase* b) const {
        if (a->mMask != b->mMask) {
            return false;
        }

        for (BindGroupIndex group : IterateBitSet(a->mMask)) {
            if (a->GetBindGroupLayout(group) != b->GetBindGroupLayout(group)) {
                return false;
            }
        }

        return true;
    }

}  // namespace dawn_native
