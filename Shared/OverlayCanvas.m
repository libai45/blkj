#import "OverlayCanvas.h"

#import <CoreText/CoreText.h>
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

static IOSurfaceRef g_surface = NULL;
static size_t g_width_px = 0;
static size_t g_height_px = 0;
static size_t g_bytes_per_row = 0;
static CGFloat g_scale = 1.0;

static inline bool oc_lock_surface(void) {
    if (!g_surface) {
        return false;
    }
    return IOSurfaceLock(g_surface, 0, NULL) == kIOReturnSuccess;
}

static inline void oc_unlock_surface(void) {
    if (g_surface) {
        IOSurfaceUnlock(g_surface, 0, NULL);
    }
}

static CGContextRef oc_create_context_locked(void) {
    if (!g_surface) {
        return NULL;
    }

    void *base = IOSurfaceGetBaseAddress(g_surface);
    if (!base) {
        return NULL;
    }

    CGColorSpaceRef color_space = CGColorSpaceCreateDeviceRGB();
    if (!color_space) {
        return NULL;
    }

    CGContextRef ctx = CGBitmapContextCreate(base,
                                             g_width_px,
                                             g_height_px,
                                             8,
                                             g_bytes_per_row,
                                             color_space,
                                             kCGImageAlphaPremultipliedFirst | kCGBitmapByteOrder32Little);
    CGColorSpaceRelease(color_space);
    return ctx;
}

static inline void oc_set_color(CGContextRef ctx, OCColor color) {
    CGContextSetRGBFillColor(ctx, color.r, color.g, color.b, color.a);
    CGContextSetRGBStrokeColor(ctx, color.r, color.g, color.b, color.a);
}

bool overlay_canvas_create(CGFloat width, CGFloat height, CGFloat scale) {
    overlay_canvas_destroy();

    if (width <= 0 || height <= 0 || scale <= 0) {
        return false;
    }

    g_scale = scale;
    g_width_px = (size_t)lrint(width * scale);
    g_height_px = (size_t)lrint(height * scale);
    g_bytes_per_row = g_width_px * 4;

    NSDictionary *props = @{
        (id)kIOSurfaceWidth: @(g_width_px),
        (id)kIOSurfaceHeight: @(g_height_px),
        (id)kIOSurfaceBytesPerElement: @4,
        (id)kIOSurfaceBytesPerRow: @(g_bytes_per_row),
        (id)kIOSurfacePixelFormat: @((uint32_t)'BGRA')
    };

    g_surface = IOSurfaceCreate((__bridge CFDictionaryRef)props);
    if (!g_surface) {
        g_width_px = 0;
        g_height_px = 0;
        g_bytes_per_row = 0;
        return false;
    }

    overlay_canvas_clear();
    return true;
}

void overlay_canvas_destroy(void) {
    if (g_surface) {
        CFRelease(g_surface);
        g_surface = NULL;
    }
    g_width_px = 0;
    g_height_px = 0;
    g_bytes_per_row = 0;
    g_scale = 1.0;
}

void overlay_canvas_clear(void) {
    if (!oc_lock_surface()) {
        return;
    }

    CGContextRef ctx = oc_create_context_locked();
    if (ctx) {
        CGContextClearRect(ctx, CGRectMake(0, 0, (CGFloat)g_width_px, (CGFloat)g_height_px));
        CGContextRelease(ctx);
    }

    oc_unlock_surface();
}

void draw_rect(CGFloat x, CGFloat y, CGFloat w, CGFloat h, OCColor color) {
    if (w <= 0 || h <= 0 || !oc_lock_surface()) {
        return;
    }

    CGContextRef ctx = oc_create_context_locked();
    if (ctx) {
        oc_set_color(ctx, color);
        CGRect r = CGRectMake(x * g_scale, y * g_scale, w * g_scale, h * g_scale);
        CGContextFillRect(ctx, r);
        CGContextRelease(ctx);
    }

    oc_unlock_surface();
}

void draw_line(CGFloat x1, CGFloat y1, CGFloat x2, CGFloat y2, CGFloat line_width, OCColor color) {
    if (line_width <= 0 || !oc_lock_surface()) {
        return;
    }

    CGContextRef ctx = oc_create_context_locked();
    if (ctx) {
        oc_set_color(ctx, color);
        CGContextSetLineWidth(ctx, line_width * g_scale);
        CGContextMoveToPoint(ctx, x1 * g_scale, y1 * g_scale);
        CGContextAddLineToPoint(ctx, x2 * g_scale, y2 * g_scale);
        CGContextStrokePath(ctx);
        CGContextRelease(ctx);
    }

    oc_unlock_surface();
}

void draw_text(const char *utf8_text, CGFloat x, CGFloat y, CGFloat font_size, OCColor color) {
    if (!utf8_text || utf8_text[0] == '\0' || font_size <= 0 || !oc_lock_surface()) {
        return;
    }

    CGContextRef ctx = oc_create_context_locked();
    if (ctx) {
        NSString *text = [NSString stringWithUTF8String:utf8_text];
        if (text) {
            UIFont *font = [UIFont systemFontOfSize:font_size * g_scale weight:UIFontWeightRegular];
            UIColor *uiColor = [UIColor colorWithRed:color.r green:color.g blue:color.b alpha:color.a];
            NSDictionary *attrs = @{
                NSFontAttributeName: font,
                NSForegroundColorAttributeName: uiColor
            };

            UIGraphicsPushContext(ctx);
            [text drawAtPoint:CGPointMake(x * g_scale, y * g_scale) withAttributes:attrs];
            UIGraphicsPopContext();
        }
        CGContextRelease(ctx);
    }

    oc_unlock_surface();
}

void overlay_canvas_present_to_layer(CALayer *target_layer) {
    if (!target_layer || !oc_lock_surface()) {
        return;
    }

    CGContextRef ctx = oc_create_context_locked();
    if (ctx) {
        CGImageRef image = CGBitmapContextCreateImage(ctx);
        if (image) {
            target_layer.contents = (__bridge id)image;
            target_layer.contentsScale = g_scale;
            target_layer.contentsGravity = kCAGravityTopLeft;
            CGImageRelease(image);
        }
        CGContextRelease(ctx);
    }

    oc_unlock_surface();
}

IOSurfaceRef overlay_canvas_get_surface(void) {
    return g_surface;
}
