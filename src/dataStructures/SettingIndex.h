#pragma once

enum SettingIndex {
    INVALID = -1,
    NONE = 0,

    /*
     * Config Settings
     *
     */

    /*
     * User Settings
     */

    /*
     * Interaction Settings
     */

    /*
     * Window Settings
     */

    SHOW_PATHTRACE_OUTPUT,
    SHOW_PATHTRACE_DEBUG,
    PATHTRACE_DEBUG_BUFFER_INDEX,
    SHOW_STATS_WINDOW,
    SHOW_CONTROL_PANEL,
    MENU_BAR_SIZE,
    SHOW_FILE_PATH_DIALOG,
    FILE_PATH_SHOW_ERROR,

    /*
     * Rendering Settings
     */
    DENOISE_KERNEL_SIZE,
    ATROUS_ITERATION_COUNT,
    ATROUS_COLOR_STD_DEV,
    ATROUS_POS_STD_DEV,
    ATROUS_NORMAL_STD_DEV,

    SHOW_DENOISE,
    CONTINUALLY_APPLY_POSTPROC,
    DO_DENOISE,
    DENOISER_CHOICE_INDEX,
    GAMMA_CORRECT,
    TONEMAP

};