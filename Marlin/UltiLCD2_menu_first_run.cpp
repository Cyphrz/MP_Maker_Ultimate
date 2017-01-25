#include <avr/pgmspace.h>

#include "Configuration.h"
#ifdef ENABLE_ULTILCD2
#include "Marlin.h"
#include "cardreader.h"//This code uses the card.longFilename as buffer to store data, to save memory.
#include "temperature.h"
#include "ConfigurationStore.h"
#include "UltiLCD2.h"
#include "UltiLCD2_hi_lib.h"
#include "UltiLCD2_menu_material.h"
#include "UltiLCD2_menu_first_run.h"
#include "UltiLCD2_menu_print.h"

#define BED_CENTER_ADJUST_X 92//��һ����x
#define BED_CENTER_ADJUST_Y 10//��һ����y
#define BED_LEFT_ADJUST_X 30//�ڶ�����x
#define BED_LEFT_ADJUST_Y (Y_MAX_LENGTH - 10)//��������y
#define BED_RIGHT_ADJUST_X (X_MAX_POS - 48)//��������x
#define BED_RIGHT_ADJUST_Y (Y_MAX_LENGTH - 10)//��������y
#define BED_CENTER_ADJUST_Z 15 //Z���½�����

static void lcd_menu_first_run_init_2();
static void lcd_menu_first_run_init_3();

static void lcd_menu_first_run_bed_level_center_adjust();
static void lcd_menu_first_run_bed_level_left_adjust();
static void lcd_menu_first_run_bed_level_right_adjust();
static void lcd_menu_first_run_bed_level_paper();
static void lcd_menu_first_run_bed_level_paper_center();
static void lcd_menu_first_run_bed_level_paper_left();
static void lcd_menu_first_run_bed_level_paper_right();

static void lcd_menu_first_run_material_load();
static void lcd_menu_first_run_material_load_heatup();
static void lcd_menu_first_run_material_load_insert();
static void lcd_menu_first_run_material_load_forward();
static void lcd_menu_first_run_material_load_wait();

static void lcd_menu_first_run_material_select_1();
static void lcd_menu_first_run_material_select_material();
static void lcd_menu_first_run_material_select_confirm_material();
static void lcd_menu_first_run_material_select_2();

static void lcd_menu_first_run_print_1();
static void lcd_menu_first_run_print_card_detect();

#define DRAW_PROGRESS_NR_IF_NOT_DONE(nr) do { if (!IS_FIRST_RUN_DONE()) { lcd_lib_draw_stringP((nr < 10) ? 100 : 94, 0, PSTR( #nr "/7")); } } while(0)
#define DRAW_PROGRESS_NR(nr) do { lcd_lib_draw_stringP((nr < 10) ? 100 : 94, 0, PSTR( #nr "/21")); } while(0)
#define CLEAR_PROGRESS_NR(nr) do { lcd_lib_clear_stringP((nr < 10) ? 100 : 94, 0, PSTR( #nr "/21")); } while(0)

//Run the first time you start-up the machine or after a factory reset.
void lcd_menu_first_run_init()
{
    SELECT_MAIN_MENU_ITEM(0);
    lcd_info_screen(lcd_menu_first_run_init_2, NULL, PSTR("CONTINUE"));
    DRAW_PROGRESS_NR_IF_NOT_DONE(1);
    lcd_lib_draw_string_centerP(10, PSTR("Welcome to the first"));
    lcd_lib_draw_string_centerP(20, PSTR("startup of your"));
    lcd_lib_draw_string_centerP(30, PSTR("Ultimaker! Press the"));
    lcd_lib_draw_string_centerP(40, PSTR("button to continue"));
    lcd_lib_update_screen();
}

static void homeAndParkHeadForCenterAdjustment2()
{
    add_homeing[Z_AXIS] = 0;
    enquecommand_P(PSTR("G28 Z0 X0 Y0"));
    char buffer[32];
    sprintf_P(buffer, PSTR("G1 F%i Z%i X%i Y%i"), int(homing_feedrate[0]), BED_CENTER_ADJUST_Z, BED_CENTER_ADJUST_X, BED_CENTER_ADJUST_Y);
    enquecommand(buffer);
}
//Started bed leveling from the calibration menu
void lcd_menu_first_run_start_bed_leveling()
{
    lcd_question_screen(lcd_menu_first_run_bed_level_center_adjust, homeAndParkHeadForCenterAdjustment2, PSTR("CONTINUE"), lcd_menu_main, NULL, PSTR("CANCEL"));
    lcd_lib_draw_string_centerP(10, PSTR("I will guide you"));
    lcd_lib_draw_string_centerP(20, PSTR("through the process"));
    lcd_lib_draw_string_centerP(30, PSTR("of adjusting your"));
    lcd_lib_draw_string_centerP(40, PSTR("buildplate."));
    lcd_lib_update_screen();
}

static void homeAndRaiseBed()
{
    enquecommand_P(PSTR("G28 Z0"));
    char buffer[32];
    sprintf_P(buffer, PSTR("G1 F%i Z%i"), int(homing_feedrate[0]), 35);
    enquecommand(buffer);
}

static void lcd_menu_first_run_init_2()
{
    SELECT_MAIN_MENU_ITEM(0);
    lcd_info_screen(lcd_menu_first_run_init_3, homeAndRaiseBed, PSTR("CONTINUE"));
    DRAW_PROGRESS_NR_IF_NOT_DONE(2);
    lcd_lib_draw_string_centerP(10, PSTR("Because this is the"));
    lcd_lib_draw_string_centerP(20, PSTR("first startup I will"));
    lcd_lib_draw_string_centerP(30, PSTR("walk you through"));
    lcd_lib_draw_string_centerP(40, PSTR("a first run wizard."));
    lcd_lib_update_screen();
}

static void homeAndParkHeadForCenterAdjustment()
{
    enquecommand_P(PSTR("G28 X0 Y0"));
    char buffer[32];
    sprintf_P(buffer, PSTR("G1 F%i Z%i X%i Y%i"), int(homing_feedrate[0]), 35, BED_CENTER_ADJUST_X, BED_CENTER_ADJUST_Y);
    enquecommand(buffer);
}

static void lcd_menu_first_run_init_3()
{
    SELECT_MAIN_MENU_ITEM(0);
    lcd_info_screen(lcd_menu_first_run_bed_level_center_adjust, homeAndParkHeadForCenterAdjustment, PSTR("CONTINUE"));
    DRAW_PROGRESS_NR_IF_NOT_DONE(3);
    lcd_lib_draw_string_centerP(10, PSTR("After transportation"));
    lcd_lib_draw_string_centerP(20, PSTR("we need to do some"));
    lcd_lib_draw_string_centerP(30, PSTR("adjustments, we are"));
    lcd_lib_draw_string_centerP(40, PSTR("going to do that now."));
    lcd_lib_update_screen();
}

static void parkHeadForLeftAdjustment()
{
    add_homeing[Z_AXIS] -= current_position[Z_AXIS];
    current_position[Z_AXIS] = 0;
    plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);

    char buffer[32];
    sprintf_P(buffer, PSTR("G1 F%i Z5"), int(homing_feedrate[Z_AXIS]));
    enquecommand(buffer);
    sprintf_P(buffer, PSTR("G1 F%i X%i Y%i"), int(homing_feedrate[X_AXIS]), BED_LEFT_ADJUST_X, BED_LEFT_ADJUST_Y);
    enquecommand(buffer);
    sprintf_P(buffer, PSTR("G1 F%i Z0"), int(homing_feedrate[Z_AXIS]));
    enquecommand(buffer);
}

static void lcd_menu_first_run_bed_level_center_adjust()
{
    LED_GLOW();

    if (lcd_lib_encoder_pos == ENCODER_NO_SELECTION)
        lcd_lib_encoder_pos = 0;

    if (printing_state == PRINT_STATE_NORMAL && lcd_lib_encoder_pos != 0 && movesplanned() < 4)
    {
        current_position[Z_AXIS] -= float(lcd_lib_encoder_pos) * 0.05;
        plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 60, 0);
    }
    lcd_lib_encoder_pos = 0;

    if (movesplanned() > 0)
        lcd_info_screen(NULL, NULL, PSTR("CONTINUE"));
    else
        lcd_info_screen(lcd_menu_first_run_bed_level_left_adjust, parkHeadForLeftAdjustment, PSTR("CONTINUE"));
    //DRAW_PROGRESS_NR_IF_NOT_DONE(4);
	lcd_lib_draw_stringP(100, 0, PSTR("1/7"));
    lcd_lib_draw_string_centerP(10, PSTR("Rotate the button"));
    lcd_lib_draw_string_centerP(20, PSTR("until the nozzle is"));
    lcd_lib_draw_string_centerP(30, PSTR("a millimeter away"));
    lcd_lib_draw_string_centerP(40, PSTR("from the buildplate."));
    lcd_lib_update_screen();
}

static void parkHeadForRightAdjustment()
{
    char buffer[32];
    sprintf_P(buffer, PSTR("G1 F%i Z5"), int(homing_feedrate[Z_AXIS]));
    enquecommand(buffer);
    sprintf_P(buffer, PSTR("G1 F%i X%i Y%i"), int(homing_feedrate[X_AXIS]), BED_RIGHT_ADJUST_X, BED_RIGHT_ADJUST_Y);
    enquecommand(buffer);
    sprintf_P(buffer, PSTR("G1 F%i Z0"), int(homing_feedrate[Z_AXIS]));
    enquecommand(buffer);
}

static void lcd_menu_first_run_bed_level_left_adjust()
{
    LED_GLOW();
    SELECT_MAIN_MENU_ITEM(0);

    lcd_info_screen(lcd_menu_first_run_bed_level_right_adjust, parkHeadForRightAdjustment, PSTR("CONTINUE"));
    //DRAW_PROGRESS_NR_IF_NOT_DONE(5);
	lcd_lib_draw_stringP(100, 0, PSTR("2/7"));
    lcd_lib_draw_string_centerP(10, PSTR("Turn left buildplate"));
    lcd_lib_draw_string_centerP(20, PSTR("screw till the nozzle"));
    lcd_lib_draw_string_centerP(30, PSTR("is a millimeter away"));
    lcd_lib_draw_string_centerP(40, PSTR("from the buildplate."));

    lcd_lib_update_screen();
}

static void lcd_menu_first_run_bed_level_right_adjust()
{
    LED_GLOW();
    SELECT_MAIN_MENU_ITEM(0);
    lcd_info_screen(lcd_menu_first_run_bed_level_paper, NULL, PSTR("CONTINUE"));
    //DRAW_PROGRESS_NR_IF_NOT_DONE(6);
	lcd_lib_draw_stringP(100, 0, PSTR("3/7"));
    lcd_lib_draw_string_centerP(10, PSTR("Turn right buildplate"));
    lcd_lib_draw_string_centerP(20, PSTR("screw till the nozzle"));
    lcd_lib_draw_string_centerP(30, PSTR("is a millimeter away"));
    lcd_lib_draw_string_centerP(40, PSTR("from the buildplate."));

    lcd_lib_update_screen();
}

static void parkHeadForCenterAdjustment()
{
    char buffer[32];
    sprintf_P(buffer, PSTR("G1 F%i Z5"), int(homing_feedrate[Z_AXIS]));
    enquecommand(buffer);
    sprintf_P(buffer, PSTR("G1 F%i X%i Y%i"), int(homing_feedrate[X_AXIS]), BED_CENTER_ADJUST_X, BED_CENTER_ADJUST_Y);
    enquecommand(buffer);
    sprintf_P(buffer, PSTR("G1 F%i Z0"), int(homing_feedrate[Z_AXIS]));
    enquecommand(buffer);
}

static void lcd_menu_first_run_bed_level_paper()
{
    SELECT_MAIN_MENU_ITEM(0);
    lcd_info_screen(lcd_menu_first_run_bed_level_paper_center, parkHeadForCenterAdjustment, PSTR("CONTINUE"));
    //DRAW_PROGRESS_NR_IF_NOT_DONE(7);
	lcd_lib_draw_stringP(100, 0, PSTR("4/7"));
    lcd_lib_draw_string_centerP(10, PSTR("Repeat this step, but"));
    lcd_lib_draw_string_centerP(20, PSTR("now use a sheet of"));
    lcd_lib_draw_string_centerP(30, PSTR("paper to fine-tune"));
    lcd_lib_draw_string_centerP(40, PSTR("the buildplate level."));
    lcd_lib_update_screen();
}

static void lcd_menu_first_run_bed_level_paper_center()
{
    LED_GLOW();

    if (lcd_lib_encoder_pos == ENCODER_NO_SELECTION)
        lcd_lib_encoder_pos = 0;

    if (printing_state == PRINT_STATE_NORMAL && lcd_lib_encoder_pos != 0 && movesplanned() < 4)
    {
        current_position[Z_AXIS] -= float(lcd_lib_encoder_pos) * 0.05;
        lcd_lib_encoder_pos = 0;
        plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 60, 0);
    }

    if (movesplanned() > 0)
        lcd_info_screen(NULL, NULL, PSTR("CONTINUE"));
    else
        lcd_info_screen(lcd_menu_first_run_bed_level_paper_left, parkHeadForLeftAdjustment, PSTR("CONTINUE"));
    //DRAW_PROGRESS_NR_IF_NOT_DONE(8);
	lcd_lib_draw_stringP(100, 0, PSTR("5/7"));
    lcd_lib_draw_string_centerP(10, PSTR("Slide a paper between"));
    lcd_lib_draw_string_centerP(20, PSTR("buildplate and nozzle"));
    lcd_lib_draw_string_centerP(30, PSTR("until you feel a"));
    lcd_lib_draw_string_centerP(40, PSTR("bit resistance."));
    lcd_lib_update_screen();
}

static void lcd_menu_first_run_bed_level_paper_left()
{
    LED_GLOW();

    SELECT_MAIN_MENU_ITEM(0);
    lcd_info_screen(lcd_menu_first_run_bed_level_paper_right, parkHeadForRightAdjustment, PSTR("CONTINUE"));
	lcd_lib_draw_stringP(100, 0, PSTR("6/7"));
	//DRAW_PROGRESS_NR_IF_NOT_DONE(9);
    lcd_lib_draw_string_centerP(20, PSTR("Repeat this for"));
    lcd_lib_draw_string_centerP(30, PSTR("the left corner..."));
    lcd_lib_update_screen();
}

static void homeBed()
{
    add_homeing[Z_AXIS] += LEVELING_OFFSET;  //Adjust the Z homing position to account for the thickness of the paper.
    // now that we are finished, save the settings to EEPROM
    Config_StoreSettings();
    enquecommand_P(PSTR("G28 X0 Y0 Z0"));
	enquecommand_P(PSTR("G1 Z15"));
}

static void lcd_menu_first_run_bed_level_paper_right()
{
    LED_GLOW();

    SELECT_MAIN_MENU_ITEM(0);
    //if (IS_FIRST_RUN_DONE())
    if(1)
        lcd_info_screen(lcd_menu_main, homeBed, PSTR("DONE"));
    else
        lcd_info_screen(lcd_menu_first_run_material_load, homeBed, PSTR("CONTINUE"));
    //DRAW_PROGRESS_NR_IF_NOT_DONE(10);
	lcd_lib_draw_stringP(100, 0, PSTR("7/7"));
	lcd_lib_draw_string_centerP(20, PSTR("Repeat this for"));
    lcd_lib_draw_string_centerP(30, PSTR("the right corner..."));
    lcd_lib_update_screen();
}

static void parkHeadForHeating()
{
    lcd_material_reset_defaults();
    enquecommand_P(PSTR("G1 F12000 X110 Y10"));
    enquecommand_P(PSTR("M84"));//Disable motor power.
}

static void lcd_menu_first_run_material_load()
{
    SELECT_MAIN_MENU_ITEM(0);
    lcd_info_screen(lcd_menu_first_run_material_load_heatup, parkHeadForHeating, PSTR("CONTINUE"));
    DRAW_PROGRESS_NR(11);
    lcd_lib_draw_string_centerP(10, PSTR("Now that we leveled"));
    lcd_lib_draw_string_centerP(20, PSTR("the buildplate"));
    lcd_lib_draw_string_centerP(30, PSTR("the next step is"));
    lcd_lib_draw_string_centerP(40, PSTR("to insert material."));
    lcd_lib_update_screen();
}

static void lcd_menu_first_run_material_load_heatup()
{
    setTargetHotend(230, 0);
    int16_t temp = degHotend(0) - 20;
    int16_t target = degTargetHotend(0) - 10 - 20;
    if (temp < 0) temp = 0;
    if (temp > target)
    {
        for(uint8_t e=0; e<EXTRUDERS; e++)
            volume_to_filament_length[e] = 1.0;//Set the extrusion to 1mm per given value, so we can move the filament a set distance.

        currentMenu = lcd_menu_first_run_material_load_insert;
        temp = target;
    }

    uint8_t progress = uint8_t(temp * 125 / target);
    if (progress < minProgress)
        progress = minProgress;
    else
        minProgress = progress;

    lcd_basic_screen();
    DRAW_PROGRESS_NR(12);
    lcd_lib_draw_string_centerP(10, PSTR("Please wait,"));
    lcd_lib_draw_string_centerP(20, PSTR("printhead heating for"));
    lcd_lib_draw_string_centerP(30, PSTR("material loading"));

    lcd_progressbar(progress);

    lcd_lib_update_screen();
}

static void runMaterialForward()
{
    //Override the max feedrate and acceleration values to get a better insert speed and speedup/slowdown
    float old_max_feedrate_e = max_feedrate[E_AXIS];
    float old_retract_acceleration = retract_acceleration;
    max_feedrate[E_AXIS] = FILAMENT_INSERT_FAST_SPEED;
    retract_acceleration = FILAMENT_LONG_MOVE_ACCELERATION;

    current_position[E_AXIS] = 0;
    plan_set_e_position(current_position[E_AXIS]);
    current_position[E_AXIS] = FILAMENT_FORWARD_LENGTH;
    plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], FILAMENT_INSERT_FAST_SPEED, 0);

    //Put back origonal values.
    max_feedrate[E_AXIS] = old_max_feedrate_e;
    retract_acceleration = old_retract_acceleration;
}

static void lcd_menu_first_run_material_load_insert()
{
    LED_GLOW();

    if (movesplanned() < 2)
    {
        current_position[E_AXIS] += 0.5;
        plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], FILAMENT_INSERT_SPEED, 0);
    }

    SELECT_MAIN_MENU_ITEM(0);
    lcd_info_screen(lcd_menu_first_run_material_load_forward, runMaterialForward, PSTR("CONTINUE"));
    DRAW_PROGRESS_NR(13);
    lcd_lib_draw_string_centerP(10, PSTR("Insert new material"));
    lcd_lib_draw_string_centerP(20, PSTR("from the rear of"));
    lcd_lib_draw_string_centerP(30, PSTR("your Ultimaker2,"));
    lcd_lib_draw_string_centerP(40, PSTR("above the arrow."));
    lcd_lib_update_screen();
}

static void lcd_menu_first_run_material_load_forward()
{
    lcd_basic_screen();
    DRAW_PROGRESS_NR(14);
    lcd_lib_draw_string_centerP(20, PSTR("Loading material..."));

    if (!blocks_queued())
    {
        lcd_lib_beep();
        led_glow_dir = led_glow = 0;
        digipot_current(2, motor_current_setting[2]*2/3);//Set E motor power lower so the motor will skip instead of grind.
        currentMenu = lcd_menu_first_run_material_load_wait;
        SELECT_MAIN_MENU_ITEM(0);
    }

    long pos = st_get_position(E_AXIS);
    long targetPos = lround(FILAMENT_FORWARD_LENGTH*axis_steps_per_unit[E_AXIS]);
    uint8_t progress = (pos * 125 / targetPos);
    lcd_progressbar(progress);

    lcd_lib_update_screen();
}

static void lcd_menu_first_run_material_load_wait()
{
    LED_GLOW();

    lcd_info_screen(lcd_menu_first_run_material_select_1, doCooldown, PSTR("CONTINUE"));
    DRAW_PROGRESS_NR(15);
    lcd_lib_draw_string_centerP(10, PSTR("Push button when"));
    lcd_lib_draw_string_centerP(20, PSTR("material exits"));
    lcd_lib_draw_string_centerP(30, PSTR("from nozzle..."));

    if (movesplanned() < 2)
    {
        current_position[E_AXIS] += 0.5;
        plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], FILAMENT_INSERT_EXTRUDE_SPEED, 0);
    }

    lcd_lib_update_screen();
}

static void lcd_menu_first_run_material_select_1()
{
    if (eeprom_read_byte(EEPROM_MATERIAL_COUNT_OFFSET()) == 1)
    {
        digipot_current(2, motor_current_setting[2]);//Set E motor power to default.
        
        for(uint8_t e=0; e<EXTRUDERS; e++)
            lcd_material_set_material(0, e);
        SET_FIRST_RUN_DONE();
        
        currentMenu = lcd_menu_first_run_print_1;
        return;
    }
    SELECT_MAIN_MENU_ITEM(0);
    lcd_info_screen(lcd_menu_first_run_material_select_material, doCooldown, PSTR("READY"));
    DRAW_PROGRESS_NR(16);
    lcd_lib_draw_string_centerP(10, PSTR("Next, select the"));
    lcd_lib_draw_string_centerP(20, PSTR("material you have"));
    lcd_lib_draw_string_centerP(30, PSTR("inserted in this"));
    lcd_lib_draw_string_centerP(40, PSTR("Ultimaker2."));
    lcd_lib_update_screen();
}

static char* lcd_material_select_callback(uint8_t nr)
{
    eeprom_read_block(card.longFilename, EEPROM_MATERIAL_NAME_OFFSET(nr), 8);
    return card.longFilename;
}

static void lcd_material_select_details_callback(uint8_t nr)
{
    lcd_lib_draw_stringP(5, 53, PSTR("Select the material"));
}

static void lcd_menu_first_run_material_select_material()
{
    LED_GLOW();
    uint8_t count = eeprom_read_byte(EEPROM_MATERIAL_COUNT_OFFSET());

    lcd_scroll_menu(PSTR("MATERIAL"), count, lcd_material_select_callback, lcd_material_select_details_callback);
    CLEAR_PROGRESS_NR(17);
    lcd_lib_update_screen();

    if (lcd_lib_button_pressed)
    {
        digipot_current(2, motor_current_setting[2]);//Set E motor power to default.

        for(uint8_t e=0; e<EXTRUDERS; e++)
            lcd_material_set_material(SELECTED_SCROLL_MENU_ITEM(), e);
        SET_FIRST_RUN_DONE();
        lcd_change_to_menu(lcd_menu_first_run_material_select_confirm_material);
        strcat_P(card.longFilename, PSTR(" as material,"));
    }
}

static void lcd_menu_first_run_material_select_confirm_material()
{
    LED_GLOW();
    lcd_question_screen(lcd_menu_first_run_material_select_2, NULL, PSTR("YES"), lcd_menu_first_run_material_select_material, NULL, PSTR("NO"));
    DRAW_PROGRESS_NR(18);
    lcd_lib_draw_string_centerP(20, PSTR("You have chosen"));
    lcd_lib_draw_string_center(30, card.longFilename);
    lcd_lib_draw_string_centerP(40, PSTR("is this right?"));
    lcd_lib_update_screen();
}

static void lcd_menu_first_run_material_select_2()
{
    SELECT_MAIN_MENU_ITEM(0);
    lcd_info_screen(lcd_menu_first_run_print_1, NULL, PSTR("CONTINUE"));
    DRAW_PROGRESS_NR(19);
    lcd_lib_draw_string_centerP(10, PSTR("Now your Ultimaker2"));
    lcd_lib_draw_string_centerP(20, PSTR("knows what kind"));
    lcd_lib_draw_string_centerP(30, PSTR("of material"));
    lcd_lib_draw_string_centerP(40, PSTR("it is using."));
    lcd_lib_update_screen();
}

static void lcd_menu_first_run_print_1()
{
    SELECT_MAIN_MENU_ITEM(0);
    lcd_info_screen(lcd_menu_first_run_print_card_detect, NULL, PSTR("ARE YOU READY?"));
    DRAW_PROGRESS_NR(20);
    lcd_lib_draw_string_centerP(20, PSTR("I'm ready let's"));
    lcd_lib_draw_string_centerP(30, PSTR("make a 3D Print!"));
    lcd_lib_update_screen();
}

static void lcd_menu_first_run_print_card_detect()
{
    if (!card.sdInserted)
    {
        lcd_info_screen(lcd_menu_main);
        DRAW_PROGRESS_NR(21);
        lcd_lib_draw_string_centerP(20, PSTR("Please insert SD-card"));
        lcd_lib_draw_string_centerP(30, PSTR("that came with"));
        lcd_lib_draw_string_centerP(40, PSTR("your Ultimaker2..."));
        lcd_lib_update_screen();
        card.release();
        return;
    }

    if (!card.isOk())
    {
        lcd_info_screen(lcd_menu_main);
        DRAW_PROGRESS_NR(21);
        lcd_lib_draw_string_centerP(30, PSTR("Reading card..."));
        lcd_lib_update_screen();
        card.initsd();
        return;
    }

    SELECT_MAIN_MENU_ITEM(0);
    lcd_info_screen(lcd_menu_print_select, NULL, PSTR("LET'S PRINT"));
    DRAW_PROGRESS_NR(21);
    lcd_lib_draw_string_centerP(10, PSTR("Select a print file"));
    lcd_lib_draw_string_centerP(20, PSTR("on the SD-card"));
    lcd_lib_draw_string_centerP(30, PSTR("and press the button"));
    lcd_lib_draw_string_centerP(40, PSTR("to print it!"));
    lcd_lib_update_screen();
}
#endif//ENABLE_ULTILCD2
