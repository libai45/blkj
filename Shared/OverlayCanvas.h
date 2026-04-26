#ifndef OVERLAY_CANVAS_H
#define OVERLAY_CANVAS_H

#include <CoreGraphics/CoreGraphics.h>
#include <IOSurface/IOSurfaceRef.h>
#include <QuartzCore/QuartzCore.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    CGFloat r;
    CGFloat g;
    CGFloat b;
    CGFloat a;
} OCColor;

/**
 * 创建透明画布（App 内）。
 * width/height: 点（pt）
 * scale: 屏幕缩放（通常传 UIScreen.mainScreen.scale）
 */
bool overlay_canvas_create(CGFloat width, CGFloat height, CGFloat scale);

/**
 * 销毁画布并释放资源。
 */
void overlay_canvas_destroy(void);

/**
 * 清空画布（透明背景）。
 */
void overlay_canvas_clear(void);

/**
 * 绘制实心矩形。
 */
void draw_rect(CGFloat x, CGFloat y, CGFloat w, CGFloat h, OCColor color);

/**
 * 绘制线段。
 */
void draw_line(CGFloat x1, CGFloat y1, CGFloat x2, CGFloat y2, CGFloat line_width, OCColor color);

/**
 * 绘制文本。
 * font_size: 字体大小（pt）
 */
void draw_text(const char *utf8_text, CGFloat x, CGFloat y, CGFloat font_size, OCColor color);

/**
 * 把当前画布提交到目标 CALayer。
 * 建议目标 layer.backgroundColor = UIColor.clearColor.CGColor。
 */
void overlay_canvas_present_to_layer(CALayer *target_layer);

/**
 * 获取底层 IOSurface（只读访问，勿释放）。
 */
IOSurfaceRef overlay_canvas_get_surface(void);

#ifdef __cplusplus
}
#endif

#endif // OVERLAY_CANVAS_H
