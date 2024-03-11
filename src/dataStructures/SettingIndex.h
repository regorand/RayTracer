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

    /*
     * Rendering Settings
     */
    DENOISE_KERNEL_SIZE,

    CONTINUALLY_APPLY_POSTPROC,
    DO_DENOISE,
    DENOISER_CHOICE_INDEX,
    GAMMA_CORRECT,
    TONEMAP

};