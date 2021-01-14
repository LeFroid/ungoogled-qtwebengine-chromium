// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "third_party/blink/renderer/platform/graphics/static_bitmap_image.h"

#include "base/rand_util.h"
#include "base/logging.h"
#include "base/numerics/checked_math.h"
#include "gpu/command_buffer/client/gles2_interface.h"
#include "third_party/blink/renderer/platform/graphics/accelerated_static_bitmap_image.h"
#include "third_party/blink/renderer/platform/graphics/graphics_context.h"
#include "third_party/blink/renderer/platform/graphics/image_observer.h"
#include "third_party/blink/renderer/platform/graphics/paint/paint_image.h"
#include "third_party/blink/renderer/platform/graphics/unaccelerated_static_bitmap_image.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkImage.h"
#include "third_party/skia/include/core/SkPaint.h"
#include "third_party/skia/include/core/SkSurface.h"
#include "third_party/skia/include/private/SkColorData.h"
#include "v8/include/v8.h"

namespace blink {

scoped_refptr<StaticBitmapImage> StaticBitmapImage::Create(
    PaintImage image,
    ImageOrientation orientation) {
  DCHECK(!image.IsTextureBacked());
  return UnacceleratedStaticBitmapImage::Create(std::move(image), orientation);
}

scoped_refptr<StaticBitmapImage> StaticBitmapImage::Create(
    sk_sp<SkData> data,
    const SkImageInfo& info,
    ImageOrientation orientation) {
  return UnacceleratedStaticBitmapImage::Create(
      SkImage::MakeRasterData(info, std::move(data), info.minRowBytes()),
      orientation);
}

IntSize StaticBitmapImage::SizeRespectingOrientation() const {
  if (orientation_.UsesWidthAsHeight())
    return Size().TransposedSize();
  else
    return Size();
}

void StaticBitmapImage::DrawHelper(
    cc::PaintCanvas* canvas,
    const PaintFlags& flags,
    const FloatRect& dst_rect,
    const FloatRect& src_rect,
    ImageClampingMode clamp_mode,
    RespectImageOrientationEnum respect_orientation,
    const PaintImage& image) {
  FloatRect adjusted_src_rect = src_rect;
  adjusted_src_rect.Intersect(SkRect::MakeWH(image.width(), image.height()));

  if (dst_rect.IsEmpty() || adjusted_src_rect.IsEmpty())
    return;  // Nothing to draw.

  cc::PaintCanvasAutoRestore auto_restore(canvas, false);
  FloatRect adjusted_dst_rect = dst_rect;
  if (respect_orientation && orientation_ != kDefaultImageOrientation) {
    canvas->save();

    // ImageOrientation expects the origin to be at (0, 0)
    canvas->translate(adjusted_dst_rect.X(), adjusted_dst_rect.Y());
    adjusted_dst_rect.SetLocation(FloatPoint());

    canvas->concat(AffineTransformToSkMatrix(
        orientation_.TransformFromDefault(adjusted_dst_rect.Size())));

    if (orientation_.UsesWidthAsHeight()) {
      adjusted_dst_rect =
          FloatRect(adjusted_dst_rect.X(), adjusted_dst_rect.Y(),
                    adjusted_dst_rect.Height(), adjusted_dst_rect.Width());
    }
  }

  canvas->drawImageRect(image, adjusted_src_rect, adjusted_dst_rect, &flags,
                        WebCoreClampingModeToSkiaRectConstraint(clamp_mode));
}

base::CheckedNumeric<size_t> StaticBitmapImage::GetSizeInBytes(
    const IntRect& rect,
    const CanvasColorParams& color_params) {
  uint8_t bytes_per_pixel = color_params.BytesPerPixel();
  base::CheckedNumeric<size_t> data_size = bytes_per_pixel;
  data_size *= rect.Size().Area();
  return data_size;
}

bool StaticBitmapImage::MayHaveStrayArea(
    scoped_refptr<StaticBitmapImage> src_image,
    const IntRect& rect) {
  if (!src_image)
    return false;

  return rect.X() < 0 || rect.Y() < 0 ||
         rect.MaxX() > src_image->Size().Width() ||
         rect.MaxY() > src_image->Size().Height();
}

bool StaticBitmapImage::CopyToByteArray(
    scoped_refptr<StaticBitmapImage> src_image,
    base::span<uint8_t> dst,
    const IntRect& rect,
    const CanvasColorParams& color_params) {
  DCHECK_EQ(dst.size(), GetSizeInBytes(rect, color_params).ValueOrDie());

  if (!src_image)
    return true;

  if (dst.size() == 0)
    return true;

  SkColorType color_type =
      (color_params.GetSkColorType() == kRGBA_F16_SkColorType)
          ? kRGBA_F16_SkColorType
          : kRGBA_8888_SkColorType;
  SkImageInfo info = SkImageInfo::Make(
      rect.Width(), rect.Height(), color_type, kUnpremul_SkAlphaType,
      color_params.GetSkColorSpaceForSkSurfaces());
  bool read_pixels_successful =
      src_image->PaintImageForCurrentFrame().readPixels(
          info, dst.data(), info.minRowBytes(), rect.X(), rect.Y());
  DCHECK(read_pixels_successful ||
         !src_image->PaintImageForCurrentFrame()
              .GetSkImageInfo()
              .bounds()
              .intersect(SkIRect::MakeXYWH(rect.X(), rect.Y(), info.width(),
                                           info.height())));

  ShuffleSubchannelColorData(dst.data(), info, rect.X(), rect.Y());
  return true;
}

// set the component to maximum-delta if it is >= maximum, or add to existing color component (color + delta)
#define shuffleComponent(color, max, delta) ( (color) >= (max) ? ((max)-(delta)) : ((color)+(delta)) )

#define writable_addr(T, p, stride, x, y) (T*)((const char *)p + y * stride + x * sizeof(T))

void StaticBitmapImage::ShuffleSubchannelColorData(const void *addr, const SkImageInfo& info, int srcX, int srcY) {
  auto w = info.width() - srcX, h = info.height() - srcY;

  // skip tiny images; info.width()/height() can also be 0
  if ((w < 8) || (h < 8)) {
    return;
  }

  // generate the first random number here
  double shuffleX = base::RandDouble();

  // cap maximum pixels to change
  auto pixels = (w + h) / 128;
  if (pixels > 10) {
    pixels = 10;
  } else if (pixels < 2) {
    pixels = 2;
  }

  auto colorType = info.colorType();
  auto fRowBytes = info.minRowBytes(); // stride

  DLOG(INFO) << "BRM: ShuffleSubchannelColorData() w=" << w << " h=" << h << " colorType=" << colorType << " fRowBytes=" << fRowBytes;

  // second random number (for y/height)
  double shuffleY = base::RandDouble();

  // calculate random coordinates using bisection
  auto currentW = w, currentH = h;
  for(;pixels >= 0; pixels--) {
    int x = currentW * shuffleX, y = currentH * shuffleY;

    // calculate randomisation amounts for each RGB component
    uint8_t shuffleR = base::RandInt(0, 4);
    uint8_t shuffleG = (shuffleR + x) % 4;
    uint8_t shuffleB = (shuffleG + y) % 4;

    // manipulate pixel data to slightly change the R, G, B components
    switch (colorType) {
      case kAlpha_8_SkColorType:
      {
         auto *pixel = writable_addr(uint8_t, addr, fRowBytes, x, y);
         auto r = SkColorGetR(*pixel), g = SkColorGetG(*pixel), b = SkColorGetB(*pixel), a = SkColorGetA(*pixel);

         r = shuffleComponent(r, UINT8_MAX-1, shuffleR);
         g = shuffleComponent(g, UINT8_MAX-1, shuffleG);
         b = shuffleComponent(b, UINT8_MAX-1, shuffleB);
         // alpha is left unchanged

         *pixel = SkColorSetARGB(a, r, g, b);
      }
      break;
      case kGray_8_SkColorType:
      {
         auto *pixel = writable_addr(uint8_t, addr, fRowBytes, x, y);
         *pixel = shuffleComponent(*pixel, UINT8_MAX-1, shuffleB);
      }
      break;
      case kRGB_565_SkColorType:
      {
         auto *pixel = writable_addr(uint16_t, addr, fRowBytes, x, y);
         unsigned    r = SkPacked16ToR32(*pixel);
         unsigned    g = SkPacked16ToG32(*pixel);
         unsigned    b = SkPacked16ToB32(*pixel);

         r = shuffleComponent(r, 31, shuffleR);
         g = shuffleComponent(g, 63, shuffleG);
         b = shuffleComponent(b, 31, shuffleB);

         unsigned r16 = (r & SK_R16_MASK) << SK_R16_SHIFT;
         unsigned g16 = (g & SK_G16_MASK) << SK_G16_SHIFT;
         unsigned b16 = (b & SK_B16_MASK) << SK_B16_SHIFT;

         *pixel = r16 | g16 | b16;
      }
      break;
      case kARGB_4444_SkColorType:
      {
         auto *pixel = writable_addr(uint16_t, addr, fRowBytes, x, y);
         auto a = SkGetPackedA4444(*pixel), r = SkGetPackedR4444(*pixel), g = SkGetPackedG4444(*pixel), b = SkGetPackedB4444(*pixel);

         r = shuffleComponent(r, 15, shuffleR);
         g = shuffleComponent(g, 15, shuffleG);
         b = shuffleComponent(b, 15, shuffleB);
         // alpha is left unchanged

         unsigned a4 = (a & 0xF) << SK_A4444_SHIFT;
         unsigned r4 = (r & 0xF) << SK_R4444_SHIFT;
         unsigned g4 = (g & 0xF) << SK_G4444_SHIFT;
         unsigned b4 = (b & 0xF) << SK_B4444_SHIFT;

         *pixel = r4 | b4 | g4 | a4;
      }
      break;
      case kRGBA_8888_SkColorType:
      {
         auto *pixel = writable_addr(uint32_t, addr, fRowBytes, x, y);
         auto a = SkGetPackedA32(*pixel), r = SkGetPackedR32(*pixel), g = SkGetPackedG32(*pixel), b = SkGetPackedB32(*pixel);

         r = shuffleComponent(r, UINT8_MAX-1, shuffleR);
         g = shuffleComponent(g, UINT8_MAX-1, shuffleG);
         b = shuffleComponent(b, UINT8_MAX-1, shuffleB);
         // alpha is left unchanged

         *pixel = (a << SK_A32_SHIFT) | (r << SK_R32_SHIFT) |
                  (g << SK_G32_SHIFT) | (b << SK_B32_SHIFT);
      }
      break;
      case kBGRA_8888_SkColorType:
      {
         auto *pixel = writable_addr(uint32_t, addr, fRowBytes, x, y);
         auto a = SkGetPackedA32(*pixel), b = SkGetPackedR32(*pixel), g = SkGetPackedG32(*pixel), r = SkGetPackedB32(*pixel);

         r = shuffleComponent(r, UINT8_MAX-1, shuffleR);
         g = shuffleComponent(g, UINT8_MAX-1, shuffleG);
         b = shuffleComponent(b, UINT8_MAX-1, shuffleB);
         // alpha is left unchanged

         *pixel = (a << SK_BGRA_A32_SHIFT) | (r << SK_BGRA_R32_SHIFT) |
                  (g << SK_BGRA_G32_SHIFT) | (b << SK_BGRA_B32_SHIFT);
      }
      break;
      default:
         // the remaining formats are not expected to be used in Chromium
         LOG(WARNING) << "BRM: ShuffleSubchannelColorData(): Ignoring pixel format";
         return;
    }

    // keep bisecting or reset current width/height as needed
    if (x == 0) {
       currentW = w;
    } else {
       currentW = x;
    }
    if (y == 0) {
       currentH = h;
    } else {
       currentH = y;
    }
  }
}

#undef writable_addr
#undef shuffleComponent

}  // namespace blink
