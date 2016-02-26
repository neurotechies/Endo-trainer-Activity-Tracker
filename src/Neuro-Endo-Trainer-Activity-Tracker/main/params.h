#ifndef PARAMS_Included
#define PARAMS_Included

#define PEGBOUNDINGBOX_X   160
#define PEGBOUNDINGBOX_Y    240
#define PEGBOUNDINGBOX_WIDTH  830
#define PEGBOUNDINGBOX_HEIGHT  550
#define PEGSATURATION_MINVALUE  120
#define MIN_PEG_SIZE  25
#define HUE_THRESHVAL_RING 20.0
#define STATIONARY  "stationary"
#define STABLE_COUNT 5
#define  IMACQ_IMGS 0 
#define  IMACQ_CAM 1 
#define  IMACQ_VID 2 
#define  IMACQ_LIVESIM 3
#define  IMACQ_STREAM 4
#define THRESHOLD 0.5
#define LEARNING_ENABLED 1
#define SHOW_OUTPUT 1
#define SHOW_NOT_CONFIDENT 1
#define SHOW_COLOR_IMAGE 0
#define SHOW_FOREGROUND 0
#define TLD_WITH_TRACKER_ONLY 0
#define STATIONARY_FRAME_COUNT 15
#define CONSECUTIVE_FRAME_COUNT 8
#define NO_OF_IMAGES_HITTING_DETECTION 10



#define SEED 0

#define LOAD_MODEL 0
#define EXPORT_MODEL_AFTER_RUN 0
#define SAVE_OUTPUT 1

#define MODEL_PATH "D:/Official/Programs/endo_tracking/opentld/OpenTLD/Result/model_DrAndre_2_0_Deg_ST_Aux"
#define  IMACQ_VIDEO_PATH  "E:/data/Endo-trainer/DrMarshal_2_0_Deg_ST_Aux.avi"
//#define  PRINT_RESULT_FILE "D:/Official/Programs/endo_tracking/opentld/OpenTLD/Result/Results_DrAnton_1_0_Deg_ST_Aux.txt"
#define  PRINT_RESULT_FILE NULL
//#define SAVE_DIRECTORY "D:/Official/Programs/endo_tracking/opentld/OpenTLD/Result/Results_DrAnton_1_0_Deg_ST_Aux.txt"
#define SAVE_DIRECTORY NULL
#define MODEL_EXPORT_FILE "D:/Official/Programs/endo_tracking/opentld/OpenTLD/Result/test"

#endif