/**
 * Copyright (c) 2019, NVIDIA CORPORATION.  All rights reserved.
 *
 * NVIDIA Corporation and its licensors retain all intellectual property
 * and proprietary rights in and to this software, related documentation
 * and any modifications thereto.  Any use, reproduction, disclosure or
 * distribution of this software and related documentation without an express
 * license agreement from NVIDIA Corporation is strictly prohibited.
 *
 */

#ifndef _NVDSMETA_NEW_H_
#define _NVDSMETA_NEW_H_

#include "glib.h"
#include "gmodule.h"
#include "nvll_osd_struct.h"

#ifdef __cplusplus
extern "C"
{
#endif

  /** number of additional fields in the metadata structure that user can use */
#define MAX_USER_FIELDS 4
  /** number of additional reserved fields in the metadata structure.
   *  It is for internal purpose */
#define MAX_RESERVED_FIELDS 4
  /** maximum array size to store the text result */
#define MAX_LABEL_SIZE 128
  /** maximum elements those can be held in given display metadata */
#define MAX_ELEMENTS_IN_DISPLAY_META 16
  /** Untracked object id*/
#define UNTRACKED_OBJECT_ID 0xFFFFFFFFFFFFFFFF

typedef GList NvDsFrameMetaList;
typedef GList NvDsUserMetaList;
typedef GList NvDsObjectMetaList;
typedef GList NvDisplayMetaList;
typedef GList NvDsClassifierMetaList;
typedef GList NvDsLabelInfoList;
typedef GList NvDsMetaList;
typedef void NvDsElementMeta;

/**
 * Specifies the type of function to copy meta data.
 * It is passed the pointer to meta data and user specific data.
 * It allocates the required memory, copies the content from src metadata
 * and returns the pointer to newly allocated memory.
 *
 * user_data is for internal purpose only. User should set it to NULL.
 */
typedef gpointer (*NvDsMetaCopyFunc) (gpointer data, gpointer user_data);
/**
 * Specifies the type of function to free meta data.
 * It is passed the pointer to meta data and user specific data.
 * It should free any resource allocated for meta data.
 *
 * user_data is for internal purpose only. User should set it to NULL.
 */
typedef void (*NvDsMetaReleaseFunc) (gpointer data, gpointer user_data);

/**
 * Specifies the type of meta data. NVIDIA defined NvDsMetaType will be present
 * in the range from NVDS_BATCH_META to NVDS_START_USER_META.
 * User can add it's own metadata type NVDS_START_USER_META onwards.
 */
typedef enum {
  NVDS_INVALID_META=-1,
  /** metadata type to be set for formed batch*/
  NVDS_BATCH_META = 1,
  /** metadata type to be set for frame */
  NVDS_FRAME_META,
  /** metadata type to be set for detected object */
  NVDS_OBJ_META,
  /** metadata type to be set for display */
  NVDS_DISPLAY_META,
  /** metadata type to be set for object classifier */
  NVDS_CLASSIFIER_META,
  /** metadata type to be set for given label of classifier */
  NVDS_LABEL_INFO_META,
  /** used for internal purpose */
  NVDS_USER_META,
  /** metadata type to be set for payload generated by
   * msg converter */
  NVDS_PAYLOAD_META,
  /** metadata type to be set for payload generated by msg broker */
  NVDS_EVENT_MSG_META,
  /** metadata type to be set for optical flow */
  NVDS_OPTICAL_FLOW_META,
  /** metadata type to be set for latency measurement */
  NVDS_LATENCY_MEASUREMENT_META,
  /** metadata type of raw inference output attached by gst-nvinfer.
   * Refer NvDsInferTensorMeta for details. */
  NVDSINFER_TENSOR_OUTPUT_META,
  /** metadata type of segmentation model output attached by gst-nvinfer.
   * Refer NvDsInferSegmentationMeta for details. */
  NVDSINFER_SEGMENTATION_META,
  /** Reserved field */
  NVDS_RESERVED_META = 4095,
  /** metadata type to be set for metadata attached by nvidia gstreamer plugins
   * before nvstreammux gstreamer plugin. It is set as user metadata inside
   * @ref NvDsFrameMeta
   * NVIDIA specific gst meta are in the range from NVDS_GST_CUSTOM_META to
   * NVDS_GST_CUSTOM_META + 4096 */
  NVDS_GST_CUSTOM_META = 4096,
  /** Start adding user specific meta types from here */
  NVDS_START_USER_META = NVDS_GST_CUSTOM_META + 4096 + 1,
  NVDS_FORCE32_META = 0x7FFFFFFF
} NvDsMetaType;

/**
 * Holds information about given metadata pool.
 */
typedef struct _NvDsMetaPool {
  /** type of the pool. Used for internal purpose */
  NvDsMetaType meta_type;
  /** max elements in the pool. Used for internal purpose */
  guint max_elements_in_pool;
  /** size of an element in the given pool. Used for internal purpose */
  guint element_size;
  /** number of empty elements. Used for internal purpose */
  guint num_empty_elements;
  /** number of filled elements. These many elemnts are in use */
  guint num_full_elements;
  /** List containing empty elements */
  NvDsMetaList * empty_list;
  /** List containing full elements */
  NvDsMetaList * full_list;
  /** copy function. Used for internal purpose */
  NvDsMetaCopyFunc copy_func;
  /** release function. Used for internal purpose */
  NvDsMetaReleaseFunc release_func;
}NvDsMetaPool;

/**
 * Holds information about base metadata of given metadata type
 */
typedef struct _NvDsBaseMeta {
  /** pointer to batch_meta*/
  struct _NvDsBatchMeta *batch_meta;
  /** metadata type of the given element */
  NvDsMetaType meta_type;
  /** pointer to user context. */
  void * uContext;
  /** copy function to be provided by the user.
   * It is called when meta_data needs to copied / transformed
   * from one buffer to other. meta_data and user_data are passed as arguments.
   */
  NvDsMetaCopyFunc copy_func;
  /** release function to be provided by the user
   * It is called when meta_data is going to be released.
   */
  NvDsMetaReleaseFunc release_func;
} NvDsBaseMeta;

/**
 * Holds information a formed batched containing the frames from different
 * sources.
 */
typedef struct _NvDsBatchMeta {
  NvDsBaseMeta base_meta;
  /** maximum number of frames those can be present the batch */
  guint max_frames_in_batch;
  /** Number of frames present in the current batch */
  guint num_frames_in_batch;
  /** pool of pointers of type “NvDsFrameMeta” */
  NvDsMetaPool *frame_meta_pool;
  /** pool of pointers of type “NvDsObjMeta” */
  NvDsMetaPool *obj_meta_pool;
  /** pool of pointers of type “NvDsClassifierMeta” */
  NvDsMetaPool *classifier_meta_pool;
  /** A pool of pointers of type “NvDsDisplayMeta” */
  NvDsMetaPool *display_meta_pool;
  /** A pool of pointers of type “NvDsUserMeta” */
  NvDsMetaPool *user_meta_pool;
  /** A pool of pointers of type “NvDsLabelInfo” */
  NvDsMetaPool *label_info_meta_pool;
  /** A list of pointers of type “NvDsFrameMeta" in use in the current batch */
  NvDsFrameMetaList *frame_meta_list;
  /** A list of pointers of type “NvDsUserMeta” in use in the current batch */
  NvDsUserMetaList *batch_user_meta_list;
  /** lock to be taken before accessing metadata to avoid simultaneous update
   * of same metadata by multiple components */
  GRecMutex meta_mutex;
  /** For additional user specific batch info */
  gint64 misc_batch_info[MAX_USER_FIELDS];
  /** For internal purpose */
  gint64 reserved[MAX_RESERVED_FIELDS];
} NvDsBatchMeta;

/**
 * Holds information of frame metadata in the batch
 */
typedef struct _NvDsFrameMeta {
   /** base metadata for frame */
  NvDsBaseMeta base_meta;
   /** pad or port index of stream muxer component for the frame in the
    * batch */
  guint pad_index;
  /** location of the frame in the batch. NvBufSurfaceParams for
   * the frame will be at index "batch_id" in the "surfaceList"
   * array of NvBufSurface. */
  guint batch_id;
  /** current frame number of the source */
  gint frame_num;
  /** pts of the frame */
  guint64 buf_pts;
  /** ntp timestamp */
  guint64 ntp_timestamp;
  /** source_id of the frame in the batch e.g. camera_id.
   * It need not be in sequential order */
  guint source_id;
  /** Number of surfaces present in this frame. This is required in case
   * multiple surfaces per frame*/
  gint num_surfaces_per_frame;
  /* width of the frame at the input of stream muxer */
  guint source_frame_width;
  /* height of the frame at the input of stream muxer */
  guint source_frame_height;
  /* surface type of sub frame. This is required in case multiple surfaces
   * per frame */
  guint surface_type;
  /* surface index of sub frame. This is required in case multiple surfaces
   * per frame */
  guint surface_index;
  /** number of object meta elements attached to the current frame */
  guint num_obj_meta;
  /** boolean indicating whether inference is performed on given frame */
  gboolean bInferDone;
  /** list of pointers of type “NvDsObjectMeta” in use for the given frame */
  NvDsObjectMetaList *obj_meta_list;
  /** list of pointers of type “NvDsDisplayMeta” in use for the given frame */
  NvDisplayMetaList *display_meta_list;
  /** list of pointers of type “NvDsUserMeta” in use for the given frame */
  NvDsUserMetaList *frame_user_meta_list;
  /** For additional user frame info */
  gint64 misc_frame_info[MAX_USER_FIELDS];
  /**For internal purpose */
  gint64 reserved[MAX_RESERVED_FIELDS];
} NvDsFrameMeta;

/**
 * Holds information of object metadata in the frame
 */
typedef struct _NvDsObjectMeta {
  NvDsBaseMeta base_meta;
  /** pointer to the parent NvDsObjectMeta. Set to NULL if parent is not
   * present */
  struct _NvDsObjectMeta *parent;
  /** unique component id that attaches NvDsObjectMeta metadata */
  gint unique_component_id;
  /** Index of the object class infered by the primary detector/classifier */
  gint class_id;
  /** Unique ID for tracking the object. @ref UNTRACKED_OBJECT_ID indicates the
   * object has not been tracked */
  guint64 object_id;
  /** confidence value of the object, set by inference component */
  gfloat confidence;
  /** Structure containing the positional parameters of the object in the frame.
   *  Can also be used to overlay borders / semi-transparent boxes on top of
   *  objects. Refer @see NvOSD_RectParams from nvll_osd_struct.h
   */
  NvOSD_RectParams rect_params;
  /** Text describing the object can be overlayed using this structure.
   *  @see NvOSD_TextParams from nvll_osd_struct.h */
  NvOSD_TextParams text_params;
  /** an array to store the string describing the class of the detected
   * object */
  gchar obj_label[MAX_LABEL_SIZE];
  /** list of pointers of type NvDsClassifierMeta*/
  NvDsClassifierMetaList *classifier_meta_list;
  /** list of pointers of type NvDsUserMeta */
  NvDsUserMetaList *obj_user_meta_list;
  /** For additional user object info */
  gint64 misc_obj_info[MAX_USER_FIELDS];
  /** For internal purpose only */
  gint64 reserved[MAX_RESERVED_FIELDS];
}NvDsObjectMeta;

/**
 * Holds information of classifier metadata in the object
 */
typedef struct _NvDsClassifierMeta {
  NvDsBaseMeta base_meta;
  /** number of outputs/labels of the classifier */
  guint num_labels;
  /** unique component id that attaches NvDsClassifierMeta metadata */
  gint unique_component_id;
  /** list of label pointers of the given class */
  NvDsLabelInfoList *label_info_list;
} NvDsClassifierMeta;

/**
 * Holds information of label metadata in the classifier
 */
typedef struct _NvDsLabelInfo {
  NvDsBaseMeta base_meta;
  /** number of classes of the given label */
  guint num_classes;
  /** an array to store the string describing the label of the classified
   * object */
  gchar result_label[MAX_LABEL_SIZE];
  /** A pointer to store the result if it exceeds MAX_LABEL_SIZE bytes */
  gchar *pResult_label;
  /** class_id of the best result */
  guint result_class_id;
  /** label_id in case of multi label classifier */
  guint label_id;
  /** probability of best result */
  gfloat result_prob;
} NvDsLabelInfo;

/**
 * Holds information of display metadata that user can specify in the frame
 */
typedef struct NvDsDisplayMeta {
  NvDsBaseMeta base_meta;
  /** number of rectangles present in display meta */
  guint num_rects;
  /** number of labels/strings present in display meta */
  guint num_labels;
  /** number of lines present in display meta */
  guint num_lines;
  /** Structure containing the positional parameters to overlay borders
   * or semi-transparent rectangles as required by the user in the frame
   *  Refer NvOSD_RectParams from nvll_osd_struct.h
   */
  NvOSD_RectParams rect_params[MAX_ELEMENTS_IN_DISPLAY_META];
  /** Text describing the user defined string can be overlayed using this
   * structure. @see NvOSD_TextParams from nvll_osd_struct.h */
  NvOSD_TextParams text_params[MAX_ELEMENTS_IN_DISPLAY_META];
  /** parameters of the line of polygon that user can draw in the frame.
   *  e.g. to set ROI in the frame by specifying the lines.
   *  Refer NvOSD_RectParams from nvll_osd_struct.h */
  NvOSD_LineParams line_params[MAX_ELEMENTS_IN_DISPLAY_META];
  /** user specific osd metadata*/
  gint64 misc_osd_data[MAX_USER_FIELDS];
  /** for internal purpose */
  gint64 reserved[MAX_RESERVED_FIELDS];
} NvDsDisplayMeta;

/**
 * Holds information of user metadata that user can specify
 */
typedef struct _NvDsUserMeta {
  NvDsBaseMeta base_meta;
  /** pointer to user data to be attached
   * Refer to deepstream-user-metadata-test example for usage */
  void *user_meta_data;
} NvDsUserMeta;

/**
 * lock to be acquired before updating metadata
 *
 * @param[in] batch_meta A pointer to @ref NvDsBatchMeta strcture
 */
void nvds_acquire_meta_lock (NvDsBatchMeta *batch_meta);

/**
 * lock to be released after updating metadata
 *
 * @param[in] batch_meta A pointer to @ref NvDsBatchMeta strcture
 */
void nvds_release_meta_lock (NvDsBatchMeta *batch_meta);

/**
 * Creates a NvDsBatchMeta of given batch size.
 *
 * @param[in] max_batch_size maximum number of frames those can be present
 *            in the batch
 * returns allocated @ref NvDsBatchMeta pointer
 */
NvDsBatchMeta *nvds_create_batch_meta(guint max_batch_size);

/**
 * Releases NvDsBatchMeta batch_meta pointer
 *
 * @param[in] batch_meta A pointer to @ref NvDsBatchMeta to be destroyed after use
 */
gboolean nvds_destroy_batch_meta(NvDsBatchMeta *batch_meta);

/**
 * Acquires @ref NvDsFrameMeta from frame_meta pool.
 * User must acquire the frame_meta from frame_meta pool to
 * fill frame metatada.
 *
 * @param[in] batch_meta A pointer to @ref NvDsBatchMeta from which
 *            @ref NvDsFrameMeta will be acquired
 *
 * returns acquired @ref NvDsFrameMeta pointer from frame meta pool
 */
NvDsFrameMeta *nvds_acquire_frame_meta_from_pool (NvDsBatchMeta *batch_meta);

/**
 * After acquiring and filling frame metadata, user must add
 * it to the batch metadata with this API
 *
 * @param[in] batch_meta A pointer to @ref NvDsBatchMeta to which @a frame_meta
 *            will be attached.
 * @param[in] frame_meta A pointer to @ref NvDsFrameMeta acquired from
 *            @a frame_meta_pool present in @ref NvDsBatchMeta.
 */
void nvds_add_frame_meta_to_batch(NvDsBatchMeta * batch_meta,
    NvDsFrameMeta * frame_meta);

/**
 * Removes given frame meta from the batch metadata
 *
 * @param[in] batch_meta A pointer to @ref NvDsBatchMeta from which @a frame_meta
 *            is to be removed.
 * @param[in] frame_meta A pointer to @ref NvDsFrameMeta to be removed from
 *            @a batch_meta.
 */
void nvds_remove_frame_meta_from_batch (NvDsBatchMeta *batch_meta,
    NvDsFrameMeta * frame_meta);

/**
 * Acquires @ref NvDsObjectMeta from the object meta pool
 * User must acquire the object meta from the object meta pool to
 * fill object metatada
 *
 * @param[in] batch_meta A pointer to @ref NvDsBatchMeta from which @ref
 *            NvDsObjectMeta will be acquired
 *
 * returns acquired @ref NvDsObjectMeta pointer from object meta pool
 */
NvDsObjectMeta *nvds_acquire_obj_meta_from_pool (NvDsBatchMeta *batch_meta);

/**
 * After acquiring and filling object metadata user must add
 * it to the frame metadata with this API
 *
 * @param[in] frame_meta A pointer to @ref NvDsFrameMeta to which @a obj_meta
 *            will be attached.
 * @param[in] obj_meta A pointer to @ref NvDsObjectMeta acquired from
 *            @a obj_meta_pool present in @ref NvDsBatchMeta.
 * @param[in] obj_parent A pointer to parent @ref NvDsObjectMeta.
 *            This will set the parent object's pointer inside @a obj_meta
 */
void nvds_add_obj_meta_to_frame(NvDsFrameMeta * frame_meta,
    NvDsObjectMeta *obj_meta, NvDsObjectMeta *obj_parent);

/**
 * Removes given @a object meta from the @a frame metadata
 *
 * @param[in] frame_meta A pointer to @ref NvDsFrameMeta from which @a obj_meta
 *            is to be removed.
 * @param[in] obj_meta A pointer to @ref NvDsObjectMeta to be removed from
 *            @a frame_meta.
 */
void nvds_remove_obj_meta_from_frame (NvDsFrameMeta * frame_meta,
    NvDsObjectMeta *obj_meta);

/**
 * Acquires @ref NvDsClassifierMeta from the classifier meta pool
 * User must acquire the classifier meta from the classifier meta pool to
 * fill classifier metatada
 *
 * @param[in] batch_meta A pointer to @ref NvDsBatchMeta from which
 *            @ref NvDsClassifierMeta will be acquired
 *
 * returns acquired @ref NvDsClassifierMeta pointer from classifier meta pool
 */
NvDsClassifierMeta *nvds_acquire_classifier_meta_from_pool(
    NvDsBatchMeta *batch_meta);

/**
 * After acquiring and filling classifier metadata user must add
 * it to the object metadata with this API
 *
 * @param[in] obj_meta A pointer to @ref NvDsObjectMeta to which @a
 *            classifier_meta will be attached.
 * @param[in] classifier_meta A pointer to @ref NvDsClassifierMeta acquired from
 *            @a classifier_meta_pool present in @ref NvDsBatchMeta.
 */

void nvds_add_classifier_meta_to_object(NvDsObjectMeta *obj_meta,
    NvDsClassifierMeta * classifier_meta);

/**
 * Removes given classifier meta from object metadata
 *
 * @param[in] obj_meta A pointer to @ref NvDsObjectMeta from which @a classifier_meta
 *            is to be removed.
 * @param[in] classifier_meta A pointer to @ref NvDsClassifierMeta to be removed from
 *            @a obj_meta.
 */
void nvds_remove_classifier_meta_from_obj (NvDsObjectMeta * obj_meta,
    NvDsClassifierMeta *classifier_meta);

/**
 * Acquires NvDsDisplayMeta from the display meta pool
 * User must acquire the display meta from the display meta pool to
 * fill display metatada
 *
 * @param[in] batch_meta A pointer to @ref NvDsBatchMeta from which @ref
 *            NvDsDisplayMeta will be acquired.
 *
 * returns acquired @ref NvDsDisplayMeta pointer from display meta pool
 */
NvDsDisplayMeta *nvds_acquire_display_meta_from_pool (
    NvDsBatchMeta *batch_meta);

/**
 * After acquiring and filling classifier metadata user must add
 * it to the frame metadata with this API
 *
 * @param[in] frame_meta A pointer to @ref NvDsFrameMeta to which @a display_meta
 *            will be attached.
 * @param[in] display_meta A pointer to @ref NvDsDisplayMeta acquired from
 *            @a display_meta_pool present in @ref NvDsBatchMeta.
 */
void nvds_add_display_meta_to_frame(NvDsFrameMeta * frame_meta,
    NvDsDisplayMeta * display_meta);

/**
 * Removes given display meta from frame metadata
 *
 * @param[in] frame_meta A pointer to @ref NvDsFrameMeta from which @a display_meta
 *            is to be removed.
 * @param[in] display_meta A pointer to @ref NvDsDisplayMeta to be removed from
 *            @a frame_meta.
 */
void nvds_remove_display_meta_from_frame (NvDsFrameMeta * frame_meta,
    NvDsDisplayMeta *display_meta);

/**
 * Acquires NvDsLabelInfo from the labelinfo meta pool
 * User must acquire the labelinfo meta from the labelinfo meta pool to
 * fill labelinfo metatada
 *
 * @param[in] batch_meta A pointer to @ref NvDsBatchMeta from which
 *            @ref NvDsLabelInfo will be acquired
 *
 * returns acquired @ref NvDsLabelInfo pointer from label info meta pool
 */
NvDsLabelInfo *nvds_acquire_label_info_meta_from_pool (
    NvDsBatchMeta *batch_meta);

/**
 * After acquiring and filling labelinfo metadata user must add
 * it to the classifier metadata with this API
 *
 * @param[in] classifier_meta A pointer to @ref NvDsClassifierMeta to which
 *            @a label_info_meta will be attached.
 * @param[in] label_info_meta A pointer to @ref NvDsLabelInfo acquired from
 *            @a label_info_meta_pool present in @ref NvDsBatchMeta.
 */
void nvds_add_label_info_meta_to_classifier(
    NvDsClassifierMeta *classifier_meta, NvDsLabelInfo * label_info_meta);

/**
 * Removes given labelinfo meta from the classifier metadata
 *
 * @param[in] classifier_meta A pointer to @ref NvDsClassifierMeta from which
 *            @a label_info_meta is to be removed.
 * @param[in] label_info_meta A pointer to @ref NvDsLabelInfo to be removed from
 *            @a classifier_meta.
 */
void nvds_remove_label_info_meta_from_classifier (
    NvDsClassifierMeta *classifier_meta, NvDsLabelInfo *label_info_meta);

/**
 * After acquiring and filling user metadata user must add
 * it to batch metadata if required at batch level with this API
 *
 * @param[in] batch_meta A pointer to @ref NvDsBatchMeta to which @a user_meta
 *            will be attached.
 * @param[in] user_meta A pointer to @ref NvDsUserMeta acquired from
 *            @a user_meta_pool present in @ref NvDsBatchMeta.
 */
void nvds_add_user_meta_to_batch(NvDsBatchMeta * batch_meta,
    NvDsUserMeta * user_meta);

/**
 * After acquiring and filling user metadata user must add
 * it to frame metadata if required at frame level with this API
 *
 * @param[in] frame_meta A pointer to @ref NvDsFrameMeta to which @a user_meta
 *            will be attached.
 * @param[in] user_meta A pointer to @ref NvDsUserMeta acquired from
 *            user_meta_pool present in @ref NvDsBatchMeta.
 */
void nvds_add_user_meta_to_frame(NvDsFrameMeta * frame_meta,
    NvDsUserMeta * user_meta);

/**
 * After acquiring and filling user metadata user must add
 * it to object metadata if required at object level with this API
 *
 * @param[in] obj_meta A pointer to @ref NvDsObjectMeta to which @a user_meta
 *            will be attached.
 * @param[in] user_meta A pointer to @ref NvDsUserMeta acquired from
 *            user_meta_pool present @ref NvDsBatchMeta.
 */
void nvds_add_user_meta_to_obj(NvDsObjectMeta * obj_meta,
    NvDsUserMeta * user_meta);

/**
 * acquires NvDsUserMeta from the user meta pool
 * User must acquire the user meta from the user meta pool to
 * fill user metatada
 *
 * @param[in] batch_meta A pointer to @ref NvDsBatchMeta from which NvDsUserMeta
 *            will be acquired
 */
NvDsUserMeta *nvds_acquire_user_meta_from_pool (NvDsBatchMeta *batch_meta);

/**
 * Removes given user metadata from the batch metadata
 *
 * @param[in] batch_meta A pointer to @ref NvDsBatchMeta from which @a user_meta
 *            is to be removed.
 * @param[in] user_meta A pointer to @ref NvDsUserMeta to be removed from
 *            batch_meta.
 *
 * returns acquired @ref NvDsUserMeta pointer from user meta pool
 */
void nvds_remove_user_meta_from_batch(NvDsBatchMeta * batch_meta,
    NvDsUserMeta * user_meta);

/**
 * Removes given user metadata from the frame metadata
 *
 * @param[in] frame_meta A pointer to @ref NvDsFrameMeta from which @a user_meta
 *            is to be removed.
 * @param[in] user_meta A pointer to @ref NvDsUserMeta to be removed from
 *            @a frame_meta.
 */
void nvds_remove_user_meta_from_frame(NvDsFrameMeta * frame_meta,
    NvDsUserMeta * user_meta);

/**
 * Removes given user metadata from the object metadata
 *
 * @param[in] obj_meta A pointer to @ref NvDsObjectMeta from which @a user_meta
 *            is to be removed.
 * @param[in] user_meta A pointer to @ref NvDsUserMeta to be removed from
 *            @a obj_meta.
 */
void nvds_remove_user_meta_from_object(NvDsObjectMeta * obj_meta,
    NvDsUserMeta * user_meta);

/**
 * Debug function to get current metadata info
 *
 * @param[in] batch_meta A pointer to @ref NvDsBatchMeta
 */
gboolean nvds_get_current_metadata_info(NvDsBatchMeta *batch_meta);

/**
 * copy function to copy batch_meta
 * It is called when meta_data needs to copied / transformed
 * from one buffer to other. meta_data and user_data are passed as arguments.
 *
 * @param[in] data A pointer of type @ref NvDsBatchMeta to be typecasted to
 *            gpointer
 * @param[in] user_data A pointer to user specific data
 *
 * return a void pointer that can be typecasted to NvDsBatchMeta
 *
 */
gpointer nvds_batch_meta_copy_func (gpointer data, gpointer user_data);

/**
 * batch_meta release function called when meta_data is going to be released.
 *
 * @param[in] data A pointer of type @ref NvDsBatchMeta to be typecasted to gpointer
 * @param[in] user_data A pointer to user specific data
 */
void nvds_batch_meta_release_func(gpointer data, gpointer user_data);

/**
 * Returns NvDsFrameMeta pointer at given index from the frame_meta_list
 *
 * @param[in] frame_meta_list A list of pointer of type NvDsFrameMeta
 * @param[in] index index at which @a NvDsFrameMeta pointer needs to be accessed.
 *
 * returns  a pointer to @ref NvDsFrameMeta from frame_meta_list
 */

NvDsFrameMeta *nvds_get_nth_frame_meta (NvDsFrameMetaList *frame_meta_list,
    guint index);

/**
 * removes all the frame metadata present in the batch metadata
 *
 * @param[in] batch_meta A pointer to @ref NvDsBatchMeta from which @a
 *            NvDsFrameMetaList needs to be cleared
 * @param[in] meta_list A pointer to @ref NvDsFrameMetaList which needs to be cleared
 */
void nvds_clear_frame_meta_list(NvDsBatchMeta *batch_meta,
    NvDsFrameMetaList *meta_list);

/**
 * removes all the object metadata present in the frame metadata
 *
 * @param[in] frame_meta A pointer to @ref NvDsFrameMeta from which @a
 *            NvDsObjectMetaList needs to be cleared
 * @param[in] meta_list A pointer to @ref NvDsObjectMetaList which needs to be
 *            cleared
 */
void nvds_clear_obj_meta_list(NvDsFrameMeta *frame_meta,
    NvDsObjectMetaList *meta_list);

/**
 * removes all the classifier metadata present in the object metadata
 *
 * @param[in] obj_meta A pointer to @ref NvDsObjectMeta from which @a
 *            NvDsClassifierMetaList needs to be cleared
 * @param[in] meta_list A pointer to @ref NvDsClassifierMetaList which needs to
 *            be cleared
 */
void nvds_clear_classifier_meta_list(NvDsObjectMeta *obj_meta,
    NvDsClassifierMetaList *meta_list);

/**
 * removes all the label info metadata present in classifier metadata
 *
 * @param[in] classifier_meta A pointer to @ref NvDsClassifierMeta from which @a
 *            NvDsLabelInfoList needs to be cleared
 * @param[in] meta_list A pointer to @ref NvDsLabelInfoList which needs to be
 *            cleared
 */
void nvds_clear_label_info_meta_list(NvDsClassifierMeta *classifier_meta,
    NvDsLabelInfoList *meta_list);

/**
 * removes all the display metadata present in the frame metadata
 *
 * @param[in] frame_meta A pointer to @ref NvDsFrameMeta from which @a
 *            NvDisplayMetaList needs to be cleared
 * @param[in] meta_list A pointer to @ref NvDisplayMetaList which needs to be
 *            cleared
 */
void nvds_clear_display_meta_list(NvDsFrameMeta *frame_meta,
    NvDisplayMetaList *meta_list);

/**
 * removes all the user metadata present in the batch metadata
 *
 * @param[in] batch_meta A pointer to @ref NvDsBatchMeta from which @a
 *            NvDsUserMetaList needs to be cleared
 * @param[in] meta_list A pointer to @ref NvDsUserMetaList which needs to be
 *            cleared
 */
void nvds_clear_batch_user_meta_list(NvDsBatchMeta *batch_meta,
    NvDsUserMetaList *meta_list);

/**
 * removes all the user metadata present in the frame metadata
 *
 * @param[in] frame_meta A pointer to @ref NvDsFrameMeta from which @a
 *            NvDsUserMetaList needs to be cleared
 * @param[in] meta_list A pointer to @ref NvDsUserMetaList which needs to be
 *            cleared
 */
void nvds_clear_frame_user_meta_list(NvDsFrameMeta *frame_meta,
    NvDsUserMetaList *meta_list);

/**
 * removes all the user metadata present in the object metadata
 *
 * @param[in] object_meta A pointer to @ref NvDsObjectMeta from which @a
 *            NvDsUserMetaList needs to be cleared
 * @param[in] meta_list A pointer to @ref NvDsUserMetaList which needs to be
 *            cleared
 */
void nvds_clear_obj_user_meta_list(NvDsObjectMeta *object_meta,
    NvDsUserMetaList *meta_list);

/**
 * removes all the metadata elements present in the given metadata list
 *
 * @param[in] batch_meta A pointer to @ref NvDsBatchMeta
 * @param[in] meta_list A pointer to NvDsMetaList which needs to be cleared
 * @param[in] meta_pool A pointer to NvDsMetaPool to which list belongs to
 *
 * returns a pointer to the updated meta list
 */
NvDsMetaList *nvds_clear_meta_list(NvDsBatchMeta *batch_meta,
    NvDsMetaList *meta_list, NvDsMetaPool *meta_pool);

/**
 * deep copy of src_frame_meta to dst_frame_meta.
 * dst_frame_meta must be acquired from dst frame meta pool
 *
 * @param[in] src_frame_meta A pointer to @ref NvDsFrameMeta
 * @param[in] dst_frame_meta A pointer to @ref NvDsFrameMeta
 */
void nvds_copy_frame_meta(NvDsFrameMeta *src_frame_meta,
    NvDsFrameMeta *dst_frame_meta);

/**
 * deep copy of src_object_meta to dst_object_meta.
 * dst_object_meta must be acquired from dst object meta pool
 *
 * @param[in] src_object_meta A pointer to @ref NvDsObjectMeta
 * @param[in] dst_object_meta A pointer to @ref NvDsObjectMeta
 */
void nvds_copy_obj_meta(NvDsObjectMeta *src_object_meta,
    NvDsObjectMeta *dst_object_meta);

/**
 * deep copy of src_classifier_meta to dst_classifier_meta.
 * dst_classifier_meta must be acquired from dst classifier meta pool
 *
 * @param[in] src_classifier_meta A pointer to @ref NvDsClassifierMeta
 * @param[in] dst_classifier_meta A pointer to @ref NvDsClassifierMeta
 */
void nvds_copy_classifier_meta(NvDsClassifierMeta *src_classifier_meta,
    NvDsClassifierMeta *dst_classifier_meta);

/**
 * deep copy of src_label_info to dst_label_info.
 * dst_label_info must be acquired from dst labelinfo meta pool
 *
 * @param[in] src_label_info A pointer to @ref NvDsLabelInfo
 * @param[in] dst_label_info A pointer to @ref NvDsLabelInfo
 */
void nvds_copy_label_info_meta(NvDsLabelInfo *src_label_info,
    NvDsLabelInfo *dst_label_info);

/**
 * deep copy of src_display_meta to dst_display_meta.
 * dst_display_meta must be acquired from dst display meta pool
 *
 * @param[in] src_display_meta A pointer to @ref NvDsDisplayMeta
 * @param[in] dst_display_meta A pointer to @ref NvDsDisplayMeta
 */
void nvds_copy_display_meta(NvDsDisplayMeta *src_display_meta,
    NvDsDisplayMeta *dst_display_meta);

/**
 * deep copy of src_user_meta_list to user meta list present in the
 * dst_batch_meta.
 *
 * @param[in] src_user_meta_list A pointer to @ref NvDsUserMetaList
 * @param[in] dst_batch_meta A pointer to @ref NvDsBatchMeta
 */
void nvds_copy_batch_user_meta_list(NvDsUserMetaList *src_user_meta_list,
    NvDsBatchMeta *dst_batch_meta);

/**
 * deep copy of src_user_meta_list to user meta list present in the
 * dst_frame_meta.
 *
 * @param[in] src_user_meta_list A pointer to @ref NvDsUserMetaList
 * @param[in] dst_frame_meta A pointer to @ref NvDsFrameMeta
 */
void nvds_copy_frame_user_meta_list(NvDsUserMetaList *src_user_meta_list,
    NvDsFrameMeta *dst_frame_meta);

/**
 * deep copy of src_user_meta_list to user meta list present in the
 * dst_object_meta.
 *
 * @param[in] src_user_meta_list A pointer to @ref NvDsUserMetaList
 * @param[in] dst_object_meta A pointer to @ref NvDsObjectMeta
 */
void nvds_copy_obj_user_meta_list(NvDsUserMetaList *src_user_meta_list,
    NvDsObjectMeta *dst_object_meta);

/**
 * deep copy of src_display_meta_list to display meta list present in the
 * dst_frame_meta.
 *
 * @param[in] src_display_meta_list A pointer to @ref NvDisplayMetaList
 * @param[in] dst_frame_meta A pointer to @ref NvDsFrameMeta
 */
void nvds_copy_display_meta_list(NvDisplayMetaList *src_display_meta_list,
    NvDsFrameMeta *dst_frame_meta);

/**
 * deep copy of src_frame_meta_list to frame meta list present in the
 * dst_batch_meta.
 *
 * @param[in] src_frame_meta_list A pointer to @ref NvDsFrameMetaList
 * @param[in] dst_batch_meta A pointer to @ref NvDsBatchMeta
 */
void nvds_copy_frame_meta_list (NvDsFrameMetaList *src_frame_meta_list,
    NvDsBatchMeta *dst_batch_meta);

/**
 * deep copy of src_obj_meta_list to frame meta list present in the
 * dst_frame_meta.
 *
 * @param[in] src_obj_meta_list A pointer to @ref NvDsObjectMetaList
 * @param[in] dst_frame_meta A pointer to @ref NvDsFrameMeta
 */
void nvds_copy_obj_meta_list(NvDsObjectMetaList *src_obj_meta_list,
    NvDsFrameMeta *dst_frame_meta);

/**
 * deep copy of src_classifier_meta_list to classifier meta list present in the
 * dst_object_meta.
 *
 * @param[in] src_classifier_meta_list A pointer to @ref NvDsClassifierMetaList
 * @param[in] dst_object_meta A pointer to @ref NvDsObjectMeta
 */
void nvds_copy_classification_list(NvDsClassifierMetaList *src_classifier_meta_list,
    NvDsObjectMeta *dst_object_meta);

/**
 * deep copy of src_label_info_list to label info meta list present in the
 * dst_classifier_meta.
 *
 * @param[in] src_label_info_list A pointer to @ref NvDsLabelInfoList
 * @param[in] dst_classifier_meta A pointer to @ref NvDsClassifierMeta
 */
void nvds_copy_label_info_list(NvDsLabelInfoList *src_label_info_list,
    NvDsClassifierMeta *dst_classifier_meta);

/**
 * generates a unique user metadata type from the given string describing
 * user specific metadata.
 *
 * @param[in] meta_descriptor A pointer to string describing metadata.
 *            The format of the string should be specified as below
 *            ORG_NAME.COMPONENT_NAME.METADATA_DESCRIPTION.
 *            e.g. (NVIDIA.NVINFER.TENSOR_METADATA)
 */
NvDsMetaType nvds_get_user_meta_type(gchar *meta_descriptor);

#ifdef __cplusplus
}
#endif
#endif
