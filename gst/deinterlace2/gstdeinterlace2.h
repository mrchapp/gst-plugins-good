/*
 * GStreamer
 * Copyright (C) 2005 Martin Eikermann <meiker@upb.de>
 * Copyright (C) 2008 Sebastian Dröge <slomo@circular-chaos.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef __GST_DEINTERLACE_2_H__
#define __GST_DEINTERLACE_2_H__

#include <liboil/liboil.h>
#include <liboil/liboilfunction.h>
#include <liboil/liboilcpu.h>

#include <gst/gst.h>
#include <gst/base/gstbasetransform.h>

G_BEGIN_DECLS

#define GST_TYPE_DEINTERLACE2 \
  (gst_deinterlace2_get_type())
#define GST_DEINTERLACE2(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_DEINTERLACE2,GstDeinterlace2))
#define GST_DEINTERLACE2_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_DEINTERLACE2,GstDeinterlace2))
#define GST_IS_DEINTERLACE2(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_DEINTERLACE2))
#define GST_IS_DEINTERLACE2_CLASS(obj) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_DEINTERLACE2))

typedef struct _GstDeinterlace2 GstDeinterlace2;
typedef struct _GstDeinterlace2Class GstDeinterlace2Class;

typedef struct deinterlace_setting_s deinterlace_setting_t;
typedef struct deinterlace_method_s deinterlace_method_t;

typedef void (*deinterlace_frame_t) (GstDeinterlace2 * object);

/*
 * This structure defines the deinterlacer plugin.
 */
struct deinterlace_method_s
{
  int version;
  const char *name;
  const char *short_name;
  int fields_required;
  int accelrequired;
  int doscalerbob;
  int numsettings;
  deinterlace_setting_t *settings;
  int scanlinemode;
  deinterlace_frame_t deinterlace_frame;
  const char *description[10];
};

#define MAX_FIELD_HISTORY 10

#define PICTURE_PROGRESSIVE 0
#define PICTURE_INTERLACED_BOTTOM 1
#define PICTURE_INTERLACED_TOP 2
#define PICTURE_INTERLACED_MASK (PICTURE_INTERLACED_BOTTOM | PICTURE_INTERLACED_TOP)

typedef void (MEMCPY_FUNC) (void *pOutput, const void *pInput, size_t nSize);

typedef struct
{
  /* pointer to the start of data for this field */
  GstBuffer *buf;
  /* see PICTURE_ flags */
  guint flags;
} GstPicture;

typedef enum
{
  GST_DEINTERLACE2_TOM,
  GST_DEINTERLACE2_GREEDY_H,
  GST_DEINTERLACE2_GREEDY_L,
  GST_DEINTERLACE2_VFIR
} GstDeinterlace2Methods;

typedef enum
{
  GST_DEINTERLACE2_ALL,         /* All (missing data is interp.) */
  GST_DEINTERLACE2_TF,          /* Top Fields Only */
  GST_DEINTERLACE2_BF           /* Bottom Fields Only */
} GstDeinterlace2Fields;

typedef enum
{
  GST_DEINTERLACE2_LAYOUT_AUTO,
  GST_DEINTERLACE2_LAYOUT_TFF,
  GST_DEINTERLACE2_LAYOUT_BFF
} GstDeinterlace2FieldLayout;

struct _GstDeinterlace2
{
  GstElement parent;

  GstPad *srcpad, *sinkpad;

  guint history_count;

  guint cpu_feature_flags;
  GstDeinterlace2FieldLayout field_layout;

  guint frame_size;
  gint frame_rate_n, frame_rate_d;

  GstDeinterlace2Fields fields;

  GstDeinterlace2Methods method_id;
  deinterlace_method_t *method;

  /* The most recent pictures 
     PictureHistory[0] is always the most recent.
     Pointers are NULL if the picture in question isn't valid, e.g. because
     the program just started or a picture was skipped.
   */
  GstPicture field_history[MAX_FIELD_HISTORY];

  /* Current overlay buffer pointer. */
  GstBuffer *out_buf;

  /* Overlay pitch (number of bytes between scanlines). */
  guint output_stride;

  /* Number of bytes of actual data in each scanline.  May be less than
     OverlayPitch since the overlay's scanlines might have alignment
     requirements.  Generally equal to FrameWidth * 2.
   */
  guint line_length;

  /* Number of pixels in each scanline. */
  gint frame_width;

  /* Number of scanlines per frame. */
  gint frame_height;

  /* Number of scanlines per field.  FrameHeight / 2, mostly for
     cleanliness so we don't have to keep dividing FrameHeight by 2.
   */
  gint field_height;

  /* distance between lines in image
     need not match the pixel width
   */
  guint field_stride;
};

struct _GstDeinterlace2Class
{
  GstElementClass parent_class;
};

GType gst_deinterlace2_get_type (void);

G_END_DECLS
#endif /* __GST_DEINTERLACE_2_H__ */