/*
 * c128ui.cc - C128-specific user interface.
 *
 * Written by
 *  Andreas Matthies <andreas.matthies@gmx.net>
 *
 * This file is part of VICE, the Versatile Commodore Emulator.
 * See README for copyright notice.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA.
 *
 */

#include "vice.h"

#include <FilePanel.h>
#include <Menu.h>
#include <MenuBar.h>
#include <MenuItem.h>
#include <Window.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(__BEOS__) && defined(WORDS_BIGENDIAN)
#include <string.h>
#endif

#include "ui_file.h"

extern "C" {
#include "archdep.h"
#include "c128model.h"
#include "c128ui.h"
#include "cartridge.h"
#include "constants.h"
#include "mouse.h"
#include "resources.h"
#include "types.h"
#include "ui.h"
#include "ui_drive.h"
#include "ui_ide64.h"
#include "ui_sid.h"
#include "ui_vicii.h"
#include "ui_video.h"
#include "util.h"
#include "video.h"
}

drive_type_t drive_type[] = {
    { "1541", DRIVE_TYPE_1541 },
    { "1541-II", DRIVE_TYPE_1541II },
    { "1570", DRIVE_TYPE_1570 },
    { "1571", DRIVE_TYPE_1571 },
    { "1571cr", DRIVE_TYPE_1571CR },
    { "1581", DRIVE_TYPE_1581 },
    { "2000", DRIVE_TYPE_2000 },
    { "4000", DRIVE_TYPE_4000 },
    { "2031", DRIVE_TYPE_2031 },
    { "2040", DRIVE_TYPE_2040 },
    { "3040", DRIVE_TYPE_3040 },
    { "4040", DRIVE_TYPE_4040 },
    { "1001", DRIVE_TYPE_1001 },
    { "8050", DRIVE_TYPE_8050 },
    { "8250", DRIVE_TYPE_8250 },
    { "None", DRIVE_TYPE_NONE },
    { NULL, 0 }
};

int drive_machine_parallel_capable = 1;

ui_menu_toggle  c128_ui_menu_toggles[] = {
    { "VICIIDoubleSize", MENU_TOGGLE_DOUBLESIZE },
    { "VICIIDoubleScan", MENU_TOGGLE_DOUBLESCAN },
    { "VICIIVideoCache", MENU_TOGGLE_VIDEOCACHE },
    { "Acia1Enable", MENU_TOGGLE_ACIA },
    { "REU", MENU_TOGGLE_REU },
    { "REUImageWrite", MENU_TOGGLE_REU_SWC },
    { "GEORAM", MENU_TOGGLE_GEORAM },
    { "GEORAMImageWrite", MENU_TOGGLE_GEORAM_SWC },
    { "RAMCART", MENU_TOGGLE_RAMCART },
    { "RAMCARTImageWrite", MENU_TOGGLE_RAMCART_SWC },
    { "DIGIMAX", MENU_TOGGLE_DIGIMAX },
    { "IEEE488", MENU_TOGGLE_IEEE488 },
    { "Mouse", MENU_TOGGLE_MOUSE },
    { "VDCDoubleSize", MENU_TOGGLE_VDC_DOUBLESIZE },
    { "VDCStretchVertical", MENU_TOGGLE_STRETCHVERTICAL },
    { "VDCDoubleScan", MENU_TOGGLE_VDC_DOUBLESCAN },
    { "VDCVideoCache", MENU_TOGGLE_VDC_VIDEOCACHE },
    { "VDC64KB", MENU_TOGGLE_VDC64KB },
    { "C128FullBanks", MENU_TOGGLE_C128FULLBANKS },
    { "SFXSoundExpander", MENU_TOGGLE_SFX_SE },
    { "SFXSoundSampler", MENU_TOGGLE_SFX_SS },
    { "EasyFlashJumper", MENU_TOGGLE_EASYFLASH_JUMPER },
    { "EasyFlashWriteCRT", MENU_TOGGLE_EASYFLASH_AUTOSAVE },
    { "MagicVoiceCartridgeEnabled", MENU_TOGGLE_MAGICVOICE },
    { "IDE64version4", MENU_TOGGLE_IDE64_V4 },
    { "ExpertCartridgeEnabled", MENU_TOGGLE_EXPERT },
    { "ExpertImageWrite", MENU_TOGGLE_EXPERT_SWC },
    { "RRFlashJumper", MENU_TOGGLE_RR_FLASH_JUMPER },
    { "RRBankJumper", MENU_TOGGLE_RR_BANK_JUMPER },
    { "RRBiosWrite",MENU_TOGGLE_SAVE_RR_FLASH },
    { NULL, 0 }
};

ui_res_possible_values c128AciaDevice[] = {
    { 1, MENU_ACIA_RS323_DEVICE_1 },
    { 2, MENU_ACIA_RS323_DEVICE_2 },
    { 3, MENU_ACIA_RS323_DEVICE_3 },
    { 4, MENU_ACIA_RS323_DEVICE_4 },
    { -1, 0 }
};

ui_res_possible_values c128AciaBase[] = {
    { 0xd700, MENU_ACIA_BASE_D700 },
    { 0xde00, MENU_ACIA_BASE_DE00 },
    { 0xdf00, MENU_ACIA_BASE_DF00 },
    { -1, 0 }
};

ui_res_possible_values c128AciaInt[] = {
    { 0, MENU_ACIA_INT_NONE },
    { 1, MENU_ACIA_INT_IRQ },
    { 2, MENU_ACIA_INT_NMI },
    { -1, 0 }
};

ui_res_possible_values c128AciaMode[] = {
    { 0, MENU_ACIA_MODE_NORMAL },
    { 1, MENU_ACIA_MODE_SWIFTLINK },
    { 2, MENU_ACIA_MODE_TURBO232 },
    { -1, 0 }
};

ui_res_possible_values c128ReuSize[] = {
    { 128, MENU_REU_SIZE_128 },
    { 256, MENU_REU_SIZE_256 },
    { 512, MENU_REU_SIZE_512 },
    { 1024, MENU_REU_SIZE_1024 },
    { 2048, MENU_REU_SIZE_2048 },
    { 4096, MENU_REU_SIZE_4096 },
    { 8192, MENU_REU_SIZE_8192 },
    { 16384, MENU_REU_SIZE_16384 },
    { -1, 0 }
};

ui_res_possible_values c128GeoRAMSize[] = {
    { 64, MENU_GEORAM_SIZE_64 },
    { 128, MENU_GEORAM_SIZE_128 },
    { 256, MENU_GEORAM_SIZE_256 },
    { 512, MENU_GEORAM_SIZE_512 },
    { 1024, MENU_GEORAM_SIZE_1024 },
    { 2048, MENU_GEORAM_SIZE_2048 },
    { 4096, MENU_GEORAM_SIZE_4096 },
    { -1, 0 }
};

ui_res_possible_values c128RamCartSize[] = {
    { 64, MENU_RAMCART_SIZE_64 },
    { 128, MENU_RAMCART_SIZE_128 },
    { -1, 0 }
};

ui_res_possible_values c128DigimaxBase[] = {
    { 0xdd00, MENU_DIGIMAX_BASE_DD00 },
    { 0xde00, MENU_DIGIMAX_BASE_DE00 },
    { 0xde20, MENU_DIGIMAX_BASE_DE20 },
    { 0xde40, MENU_DIGIMAX_BASE_DE40 },
    { 0xde60, MENU_DIGIMAX_BASE_DE60 },
    { 0xde80, MENU_DIGIMAX_BASE_DE80 },
    { 0xdea0, MENU_DIGIMAX_BASE_DEA0 },
    { 0xdec0, MENU_DIGIMAX_BASE_DEC0 },
    { 0xdee0, MENU_DIGIMAX_BASE_DEE0 },
    { 0xdf00, MENU_DIGIMAX_BASE_DF00 },
    { 0xdf20, MENU_DIGIMAX_BASE_DF20 },
    { 0xdf40, MENU_DIGIMAX_BASE_DF40 },
    { 0xdf60, MENU_DIGIMAX_BASE_DF60 },
    { 0xdf80, MENU_DIGIMAX_BASE_DF80 },
    { 0xdfa0, MENU_DIGIMAX_BASE_DFA0 },
    { 0xdfc0, MENU_DIGIMAX_BASE_DFC0 },
    { 0xdfe0, MENU_DIGIMAX_BASE_DFE0 },
    { -1, 0 }
};

ui_res_possible_values c128SFXSoundExpanderChip[] = {
    { 3526, MENU_SFX_SE_3526 },
    { 3812, MENU_SFX_SE_3812 },
    { -1, 0 }
};

ui_res_possible_values c128VDCrev[] = {
    { 0, MENU_VDC_REV_0 },
    { 1, MENU_VDC_REV_1 },
    { 2, MENU_VDC_REV_2 },
    { -1, 0 }
};

ui_res_possible_values c128mousetypes[] = {
    { MOUSE_TYPE_1351, MENU_MOUSE_TYPE_1351 },
    { MOUSE_TYPE_NEOS, MENU_MOUSE_TYPE_NEOS },
    { MOUSE_TYPE_AMIGA, MENU_MOUSE_TYPE_AMIGA },
    { MOUSE_TYPE_PADDLE, MENU_MOUSE_TYPE_PADDLE },
    { MOUSE_TYPE_CX22, MENU_MOUSE_TYPE_CX22 },
    { MOUSE_TYPE_ST, MENU_MOUSE_TYPE_ST },
    { MOUSE_TYPE_SMART, MENU_MOUSE_TYPE_SMART },
    { MOUSE_TYPE_MICROMYS, MENU_MOUSE_TYPE_MICROMYS },
    { MOUSE_TYPE_KOALAPAD, MENU_MOUSE_TYPE_KOALAPAD },
    { -1, 0 }
};

ui_res_possible_values c128mouseports[] = {
    { 1, MENU_MOUSE_PORT_JOY1 },
    { 2, MENU_MOUSE_PORT_JOY2 },
    { -1, 0 }
};

ui_res_possible_values c128viciiRenderFilters[] = {
    { VIDEO_FILTER_NONE, MENU_RENDER_FILTER_NONE },
    { VIDEO_FILTER_CRT, MENU_RENDER_FILTER_CRT_EMULATION },
    { VIDEO_FILTER_SCALE2X, MENU_RENDER_FILTER_SCALE2X },
    { -1, 0 }
};

ui_res_possible_values c128VDCRenderFilters[] = {
    { VIDEO_FILTER_NONE, MENU_VDC_RENDER_FILTER_NONE },
    { VIDEO_FILTER_CRT, MENU_VDC_RENDER_FILTER_CRT_EMULATION },
    { -1, 0 }
};

ui_res_possible_values c128ExpertModes[] = {
    { 0, MENU_EXPERT_MODE_OFF },
    { 1, MENU_EXPERT_MODE_PRG },
    { 2, MENU_EXPERT_MODE_ON },
    { -1, 0 }
};

ui_res_possible_values c128RRrevs[] = {
    { 0, MENU_RR_REV_RETRO },
    { 1, MENU_RR_REV_NORDIC },
    { -1, 0 }
};

ui_res_possible_values c128_cia1models[] = {
    { 0, MENU_CIA1_MODEL_6526_OLD },
    { 1, MENU_CIA1_MODEL_6526A_NEW },
    { -1, 0 }
};

ui_res_possible_values c128_cia2models[] = {
    { 0, MENU_CIA2_MODEL_6526_OLD },
    { 1, MENU_CIA2_MODEL_6526A_NEW },
    { -1, 0 }
};

ui_res_possible_values internal_function_rom[] = {
    { 0, MENU_C128_INTERNAL_FUNCTION_NONE },
    { 1, MENU_C128_INTERNAL_FUNCTION_ROM },
    { 2, MENU_C128_INTERNAL_FUNCTION_RAM },
    { 3, MENU_C128_INTERNAL_FUNCTION_RTC },
    { -1, 0 }
};

ui_res_possible_values external_function_rom[] = {
    { 0, MENU_C128_EXTERNAL_FUNCTION_NONE },
    { 1, MENU_C128_EXTERNAL_FUNCTION_ROM },
    { 2, MENU_C128_EXTERNAL_FUNCTION_RAM },
    { 3, MENU_C128_EXTERNAL_FUNCTION_RTC },
    { -1, 0 }
};

ui_res_value_list c128_ui_res_values[] = {
    { "Acia1Dev", c128AciaDevice },
    { "Acia1Base", c128AciaBase },
    { "Acia1Irq", c128AciaInt },
    { "Acia1Mode", c128AciaMode },
    { "REUsize", c128ReuSize },
    { "GeoRAMsize", c128GeoRAMSize },
    { "RAMCARTsize", c128RamCartSize },
    { "DIGIMAXbase", c128DigimaxBase },
    { "SFXSoundExpanderChip", c128SFXSoundExpanderChip },
    { "VICIIFilter", c128viciiRenderFilters },
    { "VDCFilter", c128VDCRenderFilters },
    { "VDCRevision", c128VDCrev },
    { "Mousetype", c128mousetypes },
    { "Mouseport", c128mouseports },
    { "ExpertCartridgeMode", c128ExpertModes },
    { "RRrevision", c128RRrevs },
    { "CIA1Model", c128_cia1models },
    { "CIA2Model", c128_cia2models },
    { "InternalFunctionROM", internal_function_rom },
    { "ExternalFunctionROM", external_function_rom },
    { NULL, NULL }
};

static int c128sidaddressbase[] = { 0xd4, 0xd7, 0xde, 0xdf, -1 };

static void c128_ui_specific(void *msg, void *window)
{
    switch (((BMessage*)msg)->what) {
        case MENU_C128_MODEL_C128_PAL:
            c128model_set(C128MODEL_C128_PAL);
            break;
        case MENU_C128_MODEL_C128DCR_PAL:
            c128model_set(C128MODEL_C128DCR_PAL);
            break;
        case MENU_C128_MODEL_C128_NTSC:
            c128model_set(C128MODEL_C128_NTSC);
            break;
        case MENU_C128_MODEL_C128DCR_NTSC:
            c128model_set(C128MODEL_C128DCR_NTSC);
            break;
        case MENU_VIDEO_VDC_SETTINGS:
            ui_video(1);
            break;
          case MENU_VICII_SETTINGS:
            ui_vicii();
            break;
        case MENU_SID_SETTINGS:
            ui_sid(c128sidaddressbase);
            break;
        case MENU_REU_FILE:
            ui_select_file(B_SAVE_PANEL, REU_FILE, (void*)0);
            break;
        case MENU_GEORAM_FILE:
            ui_select_file(B_SAVE_PANEL, GEORAM_FILE, (void*)0);
            break;
        case MENU_RAMCART_FILE:
            ui_select_file(B_SAVE_PANEL, RAMCART_FILE, (void*)0);
            break;
        case MENU_IDE64_FILE1:
            ui_select_file(B_SAVE_PANEL, IDE64_FILE1, (void*)0);
            break;
        case MENU_IDE64_SIZE1:
            ui_ide64(1);
            break;
        case MENU_IDE64_FILE2:
            ui_select_file(B_SAVE_PANEL, IDE64_FILE2, (void*)0);
            break;
        case MENU_IDE64_SIZE2:
            ui_ide64(2);
            break;
        case MENU_IDE64_FILE3:
            ui_select_file(B_SAVE_PANEL, IDE64_FILE3, (void*)0);
            break;
        case MENU_IDE64_SIZE3:
            ui_ide64(3);
            break;
        case MENU_IDE64_FILE4:
            ui_select_file(B_SAVE_PANEL, IDE64_FILE4, (void*)0);
            break;
        case MENU_IDE64_SIZE4:
            ui_ide64(4);
            break;
        case MENU_MAGICVOICE_FILE:
            ui_select_file(B_SAVE_PANEL, MAGICVOICE_FILE, (void*)0);
            break;
        case MENU_EXPERT_FILE:
            ui_select_file(B_OPEN_PANEL, EXPERT_FILE, (void*)0);
            break;
        case MENU_C128_INTERNAL_FUNCTION_FILE:
            ui_select_file(B_OPEN_PANEL, C128_INT_FUNC_FILE, (void*)0);
            break;
        case MENU_C128_EXTERNAL_FUNCTION_FILE:
            ui_select_file(B_OPEN_PANEL, C128_EXT_FUNC_FILE, (void*)0);
            break;
        case MENU_EASYFLASH_SAVE_NOW:
            if (cartridge_flush_image(CARTRIDGE_EASYFLASH) < 0) {
                ui_error("Error saving EasyFlash .crt file");
            }
            break;
        default:
            break;
    }
}

int c128ui_init(void)
{
    ui_register_machine_specific(c128_ui_specific);
    ui_register_menu_toggles(c128_ui_menu_toggles);
    ui_register_res_values(c128_ui_res_values);
    ui_update_menus();
    return 0;
}

void c128ui_shutdown(void)
{
}
