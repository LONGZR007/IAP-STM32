#include <emXGUI.h>
#include <string.h>
#include <stdio.h>
#include "x_libc.h"
#include "GUI_AppDef.h"
#include "GUI_Gyro_Dialog.h"
#include "emXGUI_JPEG.h"
#include "emxgui_png.h"

#include "./i2c/i2c.h"
#include "./mpu6050/bsp_mpu_exti.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "invensense.h"
#include "invensense_adv.h"
#include "eMPL_outputs.h"
#include "mltypes.h"
#include "mpu.h"
#include "log.h"
#include "packet.h"

#define get_tick_count   (unsigned long)GUI_GetTickCount
  
//uint8_t Update_Falg = 0;
#define BMP    1    // 1 ：使用png  0: 使用bmp
TaskHandle_t Gyro_Task_Handle;
/* 全局变量定义 */
static HDC bk_hdc;       // 背景 HDC
static HDC Pitch_hdc;    // 俯仰角刻度的 HDC 
static HDC Roll_hdc;     // 横滚角刻度的 HDC 
HWND Gyro_Main_Handle;

/* Private typedef -----------------------------------------------------------*/
/* Data read from MPL. */
#define PRINT_ACCEL     (0x01)
#define PRINT_GYRO      (0x02)
#define PRINT_QUAT      (0x04)
#define PRINT_COMPASS   (0x08)
#define PRINT_EULER     (0x10)
#define PRINT_ROT_MAT   (0x20)
#define PRINT_HEADING   (0x40)
#define PRINT_PEDO      (0x80)
#define PRINT_LINEAR_ACCEL (0x100)
#define PRINT_GRAVITY_VECTOR (0x200)

volatile uint32_t hal_timestamp = 0;
#define ACCEL_ON        (0x01)
#define GYRO_ON         (0x02)
#define COMPASS_ON      (0x04)

#define MOTION          (0)
#define NO_MOTION       (1)

/* Starting sampling rate. */
#define DEFAULT_MPU_HZ  (10)

#define FLASH_SIZE      (512)
#define FLASH_MEM_START ((void*)0x1800)

#define PEDO_READ_MS    (1000)
#define TEMP_READ_MS    (500)
#define COMPASS_READ_MS (100)
struct rx_s {
    unsigned char header[3];
    unsigned char cmd;
};
struct hal_s {
    unsigned char lp_accel_mode;
    unsigned char sensors;
    unsigned char dmp_on;
    unsigned char wait_for_tap;
    volatile unsigned char new_gyro;
    unsigned char motion_int_mode;
    unsigned long no_dmp_hz;
    unsigned long next_pedo_ms;
    unsigned long next_temp_ms;
    unsigned long next_compass_ms;
    unsigned int report;
    unsigned short dmp_features;
    struct rx_s rx;
};
static struct hal_s hal = {0};

float Pitch,Roll,Yaw;

/* USB RX binary semaphore. Actually, it's just a flag. Not included in struct
 * because it's declared extern elsewhere.
 */
volatile unsigned char rx_new;

unsigned char *mpl_key = (unsigned char*)"eMPL 5.1";

/* Platform-specific information. Kinda like a boardfile. */
struct platform_data_s {
    signed char orientation[9];
};

/* The sensors can be mounted onto the board in any orientation. The mounting
 * matrix seen below tells the MPL how to rotate the raw data from the
 * driver(s).
 * TODO: The following matrices refer to the configuration on internal test
 * boards at Invensense. If needed, please modify the matrices to match the
 * chip-to-body matrix for your particular set up.
 */
static struct platform_data_s gyro_pdata = {
    .orientation = { 1, 0, 0,
                     0, 1, 0,
                     0, 0, 1}
};

extern struct inv_sensor_cal_t sensors;
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* ---------------------------------------------------------------------------*/
/* Get data from MPL.
 * TODO: Add return values to the inv_get_sensor_type_xxx APIs to differentiate
 * between new and stale data.
 */
static void read_from_mpl(void)
{
    long data[9];
    int8_t accuracy;
    unsigned long timestamp;
    float float_data[3] = {0};

    if (inv_get_sensor_type_quat(data, &accuracy, (inv_time_t*)&timestamp)) {
       /* Sends a quaternion packet to the PC. Since this is used by the Python
        * test app to visually represent a 3D quaternion, it's sent each time
        * the MPL has new data.
        */
        eMPL_send_quat(data);

        /* Specific data packets can be sent or suppressed using USB commands. */
        if (hal.report & PRINT_QUAT)
            eMPL_send_data(PACKET_DATA_QUAT, data);
    }

    if (hal.report & PRINT_ACCEL) {
        if (inv_get_sensor_type_accel(data, &accuracy,
            (inv_time_t*)&timestamp))
            eMPL_send_data(PACKET_DATA_ACCEL, data);
    }
    if (hal.report & PRINT_GYRO) {
        if (inv_get_sensor_type_gyro(data, &accuracy,
            (inv_time_t*)&timestamp))
            eMPL_send_data(PACKET_DATA_GYRO, data);
    }
#ifdef COMPASS_ENABLED
    if (hal.report & PRINT_COMPASS) {
        if (inv_get_sensor_type_compass(data, &accuracy,
            (inv_time_t*)&timestamp))
            eMPL_send_data(PACKET_DATA_COMPASS, data);
    }
#endif
    if (hal.report & PRINT_EULER) {
        if (inv_get_sensor_type_euler(data, &accuracy,
            (inv_time_t*)&timestamp))
            eMPL_send_data(PACKET_DATA_EULER, data);
    }
		
		
		/*********发送数据到匿名四轴上位机**********/
    if(1)
    {

				unsigned long timestamp;

			
				/*获取欧拉角*/
			  if (inv_get_sensor_type_euler(data, &accuracy,(inv_time_t*)&timestamp))
						{
							//inv_get_sensor_type_euler读出的数据是Q16格式，所以左移16位.
							Pitch =data[0]*1.0/(1<<16) ;
							Roll = data[1]*1.0/(1<<16);
							Yaw = data[2]*1.0/(1<<16);

            //  GUI_DEBUG("\r\nPitch = %f\r\nRoll = %f\r\nYaw = %f\r\n", Pitch, Roll, Yaw);

              // static uint32_t tick;
              // GUI_DEBUG("update：%d", GUI_GetTickCount() - tick);
              // tick = GUI_GetTickCount();
              InvalidateRect(Gyro_Main_Handle, NULL, FALSE);    // 无效化窗口，重绘刻度
						}
						
					/*获取步数*/        
				timestamp = get_tick_count();
			}

		
		
    if (hal.report & PRINT_ROT_MAT) {
        if (inv_get_sensor_type_rot_mat(data, &accuracy,
            (inv_time_t*)&timestamp))
            eMPL_send_data(PACKET_DATA_ROT, data);
    }
    if (hal.report & PRINT_HEADING) {
        if (inv_get_sensor_type_heading(data, &accuracy,
            (inv_time_t*)&timestamp))
            eMPL_send_data(PACKET_DATA_HEADING, data);
    }
    if (hal.report & PRINT_LINEAR_ACCEL) {
        if (inv_get_sensor_type_linear_acceleration(float_data, &accuracy, (inv_time_t*)&timestamp)) {
        	MPL_LOGI("Linear Accel: %7.5f %7.5f %7.5f\r\n",
        			float_data[0], float_data[1], float_data[2]);                                        
         }
    }
    if (hal.report & PRINT_GRAVITY_VECTOR) {
            if (inv_get_sensor_type_gravity(float_data, &accuracy,
                (inv_time_t*)&timestamp))
            	MPL_LOGI("Gravity Vector: %7.5f %7.5f %7.5f\r\n",
            			float_data[0], float_data[1], float_data[2]);
    }
    if (hal.report & PRINT_PEDO) {
        unsigned long timestamp;
        timestamp = get_tick_count();
        if (timestamp > hal.next_pedo_ms) {
            hal.next_pedo_ms = timestamp + PEDO_READ_MS;
            unsigned long step_count, walk_time;
            dmp_get_pedometer_step_count(&step_count);
            dmp_get_pedometer_walk_time(&walk_time);
            MPL_LOGI("Walked %ld steps over %ld milliseconds..\n", step_count,
            walk_time);
        }
    }

    /* Whenever the MPL detects a change in motion state, the application can
     * be notified. For this example, we use an LED to represent the current
     * motion state.
     */
//    msg = inv_get_message_level_0(INV_MSG_MOTION_EVENT |
//            INV_MSG_NO_MOTION_EVENT);
//    if (msg) {
//        if (msg & INV_MSG_MOTION_EVENT) {
//            MPL_LOGI("Motion!\n");
//        } else if (msg & INV_MSG_NO_MOTION_EVENT) {
//            MPL_LOGI("No motion!\n");
//        }
//    }
}

#ifdef COMPASS_ENABLED
void send_status_compass() {
	long data[3] = { 0 };
	int8_t accuracy = { 0 };
	unsigned long timestamp;
	inv_get_compass_set(data, &accuracy, (inv_time_t*) &timestamp);
	MPL_LOGI("Compass: %7.4f %7.4f %7.4f ",
			data[0]/65536.f, data[1]/65536.f, data[2]/65536.f);
	MPL_LOGI("Accuracy= %d\r\n", accuracy);

}
#endif

/* Handle sensor on/off combinations. */
static void setup_gyro(void)
{
    unsigned char mask = 0, lp_accel_was_on = 0;
    if (hal.sensors & ACCEL_ON)
        mask |= INV_XYZ_ACCEL;
    if (hal.sensors & GYRO_ON) {
        mask |= INV_XYZ_GYRO;
        lp_accel_was_on |= hal.lp_accel_mode;
    }
#ifdef COMPASS_ENABLED
    if (hal.sensors & COMPASS_ON) {
        mask |= INV_XYZ_COMPASS;
        lp_accel_was_on |= hal.lp_accel_mode;
    }
#endif
    /* If you need a power transition, this function should be called with a
     * mask of the sensors still enabled. The driver turns off any sensors
     * excluded from this mask.
     */
    mpu_set_sensors(mask);
    mpu_configure_fifo(mask);
    if (lp_accel_was_on) {
        unsigned short rate;
        hal.lp_accel_mode = 0;
        /* Switching out of LP accel, notify MPL of new accel sampling rate. */
        mpu_get_sample_rate(&rate);
        inv_set_accel_sample_rate(1000000L / rate);
    }
}

static void tap_cb(unsigned char direction, unsigned char count)
{
    switch (direction) {
    case TAP_X_UP:
        MPL_LOGI("Tap X+ ");
        break;
    case TAP_X_DOWN:
        MPL_LOGI("Tap X- ");
        break;
    case TAP_Y_UP:
        MPL_LOGI("Tap Y+ ");
        break;
    case TAP_Y_DOWN:
        MPL_LOGI("Tap Y- ");
        break;
    case TAP_Z_UP:
        MPL_LOGI("Tap Z+ ");
        break;
    case TAP_Z_DOWN:
        MPL_LOGI("Tap Z- ");
        break;
    default:
        return;
    }
    MPL_LOGI("x%d\n", count);
    return;
}

static void android_orient_cb(unsigned char orientation)
{
	switch (orientation) {
	case ANDROID_ORIENT_PORTRAIT:
        MPL_LOGI("Portrait\n");
        break;
	case ANDROID_ORIENT_LANDSCAPE:
        MPL_LOGI("Landscape\n");
        break;
	case ANDROID_ORIENT_REVERSE_PORTRAIT:
        MPL_LOGI("Reverse Portrait\n");
        break;
	case ANDROID_ORIENT_REVERSE_LANDSCAPE:
        MPL_LOGI("Reverse Landscape\n");
        break;
	default:
		return;
	}
}

static inline void run_self_test(void)
{
    int result;
    long gyro[3], accel[3];

#if defined (MPU6500) || defined (MPU9250)
    result = mpu_run_6500_self_test(gyro, accel, 0);
#elif defined (MPU6050) || defined (MPU9150)
    result = mpu_run_self_test(gyro, accel);
#endif
    if (result == 0x7) {
	MPL_LOGI("Passed!\n");
        MPL_LOGI("accel: %7.4f %7.4f %7.4f\n",
                    accel[0]/65536.f,
                    accel[1]/65536.f,
                    accel[2]/65536.f);
        MPL_LOGI("gyro: %7.4f %7.4f %7.4f\n",
                    gyro[0]/65536.f,
                    gyro[1]/65536.f,
                    gyro[2]/65536.f);
        /* Test passed. We can trust the gyro data here, so now we need to update calibrated data*/

#ifdef USE_CAL_HW_REGISTERS
        /*
         * This portion of the code uses the HW offset registers that are in the MPUxxxx devices
         * instead of pushing the cal data to the MPL software library
         */
        unsigned char i = 0;

        for(i = 0; i<3; i++) {
        	gyro[i] = (long)(gyro[i] * 32.8f); //convert to +-1000dps
        	accel[i] *= 2048.f; //convert to +-16G
        	accel[i] = accel[i] >> 16;
        	gyro[i] = (long)(gyro[i] >> 16);
        }

        mpu_set_gyro_bias_reg(gyro);

#if defined (MPU6500) || defined (MPU9250)
        mpu_set_accel_bias_6500_reg(accel);
#elif defined (MPU6050) || defined (MPU9150)
        mpu_set_accel_bias_6050_reg(accel);
#endif
#else
        /* Push the calibrated data to the MPL library.
         *
         * MPL expects biases in hardware units << 16, but self test returns
		 * biases in g's << 16.
		 */
    	unsigned short accel_sens;
    	float gyro_sens;

		mpu_get_accel_sens(&accel_sens);
		accel[0] *= accel_sens;
		accel[1] *= accel_sens;
		accel[2] *= accel_sens;
		inv_set_accel_bias(accel, 3);
		mpu_get_gyro_sens(&gyro_sens);
		gyro[0] = (long) (gyro[0] * gyro_sens);
		gyro[1] = (long) (gyro[1] * gyro_sens);
		gyro[2] = (long) (gyro[2] * gyro_sens);
		inv_set_gyro_bias(gyro, 3);
#endif
    }
    else {
            if (!(result & 0x1))
                MPL_LOGE("Gyro failed.\n");
            if (!(result & 0x2))
                MPL_LOGE("Accel failed.\n");
            if (!(result & 0x4))
                MPL_LOGE("Compass failed.\n");
     }

}

static void handle_input(void)
{
  
    char c = USART_ReceiveData(DEBUG_USART);

    switch (c) {
    /* These commands turn off individual sensors. */
    case '8':
        hal.sensors ^= ACCEL_ON;
        setup_gyro();
        if (!(hal.sensors & ACCEL_ON))
            inv_accel_was_turned_off();
        break;
    case '9':
        hal.sensors ^= GYRO_ON;
        setup_gyro();
        if (!(hal.sensors & GYRO_ON))
            inv_gyro_was_turned_off();
        break;
#ifdef COMPASS_ENABLED
    case '0':
        hal.sensors ^= COMPASS_ON;
        setup_gyro();
        if (!(hal.sensors & COMPASS_ON))
            inv_compass_was_turned_off();
        break;
#endif
    /* The commands send individual sensor data or fused data to the PC. */
    case 'a':
        hal.report ^= PRINT_ACCEL;
        break;
    case 'g':
        hal.report ^= PRINT_GYRO;
        break;
#ifdef COMPASS_ENABLED
    case 'c':
        hal.report ^= PRINT_COMPASS;
        break;
#endif
    case 'e':
        hal.report ^= PRINT_EULER;
        break;
    case 'r':
        hal.report ^= PRINT_ROT_MAT;
        break;
    case 'q':
        hal.report ^= PRINT_QUAT;
        break;
    case 'h':
        hal.report ^= PRINT_HEADING;
        break;
    case 'i':
        hal.report ^= PRINT_LINEAR_ACCEL;
        break;
    case 'o':
        hal.report ^= PRINT_GRAVITY_VECTOR;
        break;
#ifdef COMPASS_ENABLED
	case 'w':
		send_status_compass();
		break;
#endif
    /* This command prints out the value of each gyro register for debugging.
     * If logging is disabled, this function has no effect.
     */
    case 'd':
        mpu_reg_dump();
        break;
    /* Test out low-power accel mode. */
    case 'p':
        if (hal.dmp_on)
            /* LP accel is not compatible with the DMP. */
            break;
        mpu_lp_accel_mode(20);
        /* When LP accel mode is enabled, the driver automatically configures
         * the hardware for latched interrupts. However, the MCU sometimes
         * misses the rising/falling edge, and the hal.new_gyro flag is never
         * set. To avoid getting locked in this state, we're overriding the
         * driver's configuration and sticking to unlatched interrupt mode.
         *
         * TODO: The MCU supports level-triggered interrupts.
         */
        mpu_set_int_latched(0);
        hal.sensors &= ~(GYRO_ON|COMPASS_ON);
        hal.sensors |= ACCEL_ON;
        hal.lp_accel_mode = 1;
        inv_gyro_was_turned_off();
        inv_compass_was_turned_off();
        break;
    /* The hardware self test can be run without any interaction with the
     * MPL since it's completely localized in the gyro driver. Logging is
     * assumed to be enabled; otherwise, a couple LEDs could probably be used
     * here to display the test results.
     */
    case 't':
        run_self_test();
        /* Let MPL know that contiguity was broken. */
        inv_accel_was_turned_off();
        inv_gyro_was_turned_off();
        inv_compass_was_turned_off();
        break;
    /* Depending on your application, sensor data may be needed at a faster or
     * slower rate. These commands can speed up or slow down the rate at which
     * the sensor data is pushed to the MPL.
     *
     * In this example, the compass rate is never changed.
     */
    case '1':
        if (hal.dmp_on) {
            dmp_set_fifo_rate(10);
            inv_set_quat_sample_rate(100000L);
        } else
            mpu_set_sample_rate(10);
        inv_set_gyro_sample_rate(100000L);
        inv_set_accel_sample_rate(100000L);
        break;
    case '2':
        if (hal.dmp_on) {
            dmp_set_fifo_rate(20);
            inv_set_quat_sample_rate(50000L);
        } else
            mpu_set_sample_rate(20);
        inv_set_gyro_sample_rate(50000L);
        inv_set_accel_sample_rate(50000L);
        break;
    case '3':
        if (hal.dmp_on) {
            dmp_set_fifo_rate(40);
            inv_set_quat_sample_rate(25000L);
        } else
            mpu_set_sample_rate(40);
        inv_set_gyro_sample_rate(25000L);
        inv_set_accel_sample_rate(25000L);
        break;
    case '4':
        if (hal.dmp_on) {
            dmp_set_fifo_rate(50);
            inv_set_quat_sample_rate(20000L);
        } else
            mpu_set_sample_rate(50);
        inv_set_gyro_sample_rate(20000L);
        inv_set_accel_sample_rate(20000L);
        break;
    case '5':
        if (hal.dmp_on) {
            dmp_set_fifo_rate(100);
            inv_set_quat_sample_rate(10000L);
        } else
            mpu_set_sample_rate(100);
        inv_set_gyro_sample_rate(10000L);
        inv_set_accel_sample_rate(10000L);
        break;
	case ',':
        /* Set hardware to interrupt on gesture event only. This feature is
         * useful for keeping the MCU asleep until the DMP detects as a tap or
         * orientation event.
         */
        dmp_set_interrupt_mode(DMP_INT_GESTURE);
        break;
    case '.':
        /* Set hardware to interrupt periodically. */
        dmp_set_interrupt_mode(DMP_INT_CONTINUOUS);
        break;
    case '6':
        /* Toggle pedometer display. */
        hal.report ^= PRINT_PEDO;
        break;
    case '7':
        /* Reset pedometer. */
        dmp_set_pedometer_step_count(0);
        dmp_set_pedometer_walk_time(0);
        break;
    case 'f':
        if (hal.lp_accel_mode)
            /* LP accel is not compatible with the DMP. */
            return;
        /* Toggle DMP. */
        if (hal.dmp_on) {
            unsigned short dmp_rate;
            unsigned char mask = 0;
            hal.dmp_on = 0;
            mpu_set_dmp_state(0);
            /* Restore FIFO settings. */
            if (hal.sensors & ACCEL_ON)
                mask |= INV_XYZ_ACCEL;
            if (hal.sensors & GYRO_ON)
                mask |= INV_XYZ_GYRO;
            if (hal.sensors & COMPASS_ON)
                mask |= INV_XYZ_COMPASS;
            mpu_configure_fifo(mask);
            /* When the DMP is used, the hardware sampling rate is fixed at
             * 200Hz, and the DMP is configured to downsample the FIFO output
             * using the function dmp_set_fifo_rate. However, when the DMP is
             * turned off, the sampling rate remains at 200Hz. This could be
             * handled in inv_mpu.c, but it would need to know that
             * inv_mpu_dmp_motion_driver.c exists. To avoid this, we'll just
             * put the extra logic in the application layer.
             */
            dmp_get_fifo_rate(&dmp_rate);
            mpu_set_sample_rate(dmp_rate);
            inv_quaternion_sensor_was_turned_off();
            MPL_LOGI("DMP disabled.\n");
        } else {
            unsigned short sample_rate;
            hal.dmp_on = 1;
            /* Preserve current FIFO rate. */
            mpu_get_sample_rate(&sample_rate);
            dmp_set_fifo_rate(sample_rate);
            inv_set_quat_sample_rate(1000000L / sample_rate);
            mpu_set_dmp_state(1);
            MPL_LOGI("DMP enabled.\n");
        }
        break;
    case 'm':
        /* Test the motion interrupt hardware feature. */
	#ifndef MPU6050 // not enabled for 6050 product
	hal.motion_int_mode = 1;
	#endif 
        break;

    case 'v':
        /* Toggle LP quaternion.
         * The DMP features can be enabled/disabled at runtime. Use this same
         * approach for other features.
         */
        hal.dmp_features ^= DMP_FEATURE_6X_LP_QUAT;
        dmp_enable_feature(hal.dmp_features);
        if (!(hal.dmp_features & DMP_FEATURE_6X_LP_QUAT)) {
            inv_quaternion_sensor_was_turned_off();
            MPL_LOGI("LP quaternion disabled.\n");
        } else
            MPL_LOGI("LP quaternion enabled.\n");
        break;
    default:
        break;
    }
    hal.rx.cmd = 0;
}

/* Every time new gyro data is available, this function is called in an
 * ISR context. In this example, it sets a flag protecting the FIFO read
 * function.
 */
void gyro_data_ready_cb(void)
{
    hal.new_gyro = 1;
}

void Gyro_Dispose_Task(void *p)
{
    inv_error_t result;
    unsigned char accel_fsr,  new_temp = 0;
    unsigned short gyro_rate, gyro_fsr;
    unsigned long timestamp;
    struct int_param_s int_param;

#ifdef COMPASS_ENABLED
    unsigned char new_compass = 0;
    unsigned short compass_fsr;
#endif

	EXTI_MPU_Config();
	
		// Configure I2C
	I2cMaster_Init(); 

	
	//MPU_DEBUG("F4 MPU6050 test");
	
  result = mpu_init(&int_param);
  if (result) {
      GUI_ERROR("MPU6050 Init failure");
  }

    /* If you're not using an MPU9150 AND you're not using DMP features, this
     * function will place all slaves on the primary bus.
     * mpu_set_bypass(1);
     */

  result = inv_init_mpl();
  if (result) {
      MPL_LOGE("Could not initialize MPL.\n");
  }

    /* Compute 6-axis and 9-axis quaternions. */
    inv_enable_quaternion();
    inv_enable_9x_sensor_fusion();
    /* The MPL expects compass data at a constant rate (matching the rate
     * passed to inv_set_compass_sample_rate). If this is an issue for your
     * application, call this function, and the MPL will depend on the
     * timestamps passed to inv_build_compass instead.
     *
     * inv_9x_fusion_use_timestamps(1);
     */

    /* This function has been deprecated.
     * inv_enable_no_gyro_fusion();
     */

    /* Update gyro biases when not in motion.
     * WARNING: These algorithms are mutually exclusive.
     */
    inv_enable_fast_nomot();
    /* inv_enable_motion_no_motion(); */
    /* inv_set_no_motion_time(1000); */

    /* Update gyro biases when temperature changes. */
    inv_enable_gyro_tc();

    /* This algorithm updates the accel biases when in motion. A more accurate
     * bias measurement can be made when running the self-test (see case 't' in
     * handle_input), but this algorithm can be enabled if the self-test can't
     * be executed in your application.
     *
     * inv_enable_in_use_auto_calibration();
     */
#ifdef COMPASS_ENABLED
    /* Compass calibration algorithms. */
    inv_enable_vector_compass_cal();
    inv_enable_magnetic_disturbance();
#endif
    /* If you need to estimate your heading before the compass is calibrated,
     * enable this algorithm. It becomes useless after a good figure-eight is
     * detected, so we'll just leave it out to save memory.
     * inv_enable_heading_from_gyro();
     */

    /* Allows use of the MPL APIs in read_from_mpl. */
    inv_enable_eMPL_outputs();

  result = inv_start_mpl();
  if (result == INV_ERROR_NOT_AUTHORIZED) {
      while (1) {
          MPL_LOGE("Not authorized.\n");
      }
  }
  if (result) {
      MPL_LOGE("Could not start the MPL.\n");
  }

    /* Get/set hardware configuration. Start gyro. */
    /* Wake up all sensors. */
#ifdef COMPASS_ENABLED
    mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL | INV_XYZ_COMPASS);
#else
    mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL);
#endif
    /* Push both gyro and accel data into the FIFO. */
    mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL);
    mpu_set_sample_rate(DEFAULT_MPU_HZ);
#ifdef COMPASS_ENABLED
    /* The compass sampling rate can be less than the gyro/accel sampling rate.
     * Use this function for proper power management.
     */
    mpu_set_compass_sample_rate(1000 / COMPASS_READ_MS);
#endif
    /* Read back configuration in case it was set improperly. */
    mpu_get_sample_rate(&gyro_rate);
    mpu_get_gyro_fsr(&gyro_fsr);
    mpu_get_accel_fsr(&accel_fsr);
#ifdef COMPASS_ENABLED
    mpu_get_compass_fsr(&compass_fsr);
#endif
    /* Sync driver configuration with MPL. */
    /* Sample rate expected in microseconds. */
    inv_set_gyro_sample_rate(1000000L / gyro_rate);
    inv_set_accel_sample_rate(1000000L / gyro_rate);
#ifdef COMPASS_ENABLED
    /* The compass rate is independent of the gyro and accel rates. As long as
     * inv_set_compass_sample_rate is called with the correct value, the 9-axis
     * fusion algorithm's compass correction gain will work properly.
     */
    inv_set_compass_sample_rate(COMPASS_READ_MS * 1000L);
#endif
    /* Set chip-to-body orientation matrix.
     * Set hardware units to dps/g's/degrees scaling factor.
     */
    inv_set_gyro_orientation_and_scale(
            inv_orientation_matrix_to_scalar(gyro_pdata.orientation),
            (long)gyro_fsr<<15);
    inv_set_accel_orientation_and_scale(
            inv_orientation_matrix_to_scalar(gyro_pdata.orientation),
            (long)accel_fsr<<15);
#ifdef COMPASS_ENABLED
    inv_set_compass_orientation_and_scale(
            inv_orientation_matrix_to_scalar(compass_pdata.orientation),
            (long)compass_fsr<<15);
#endif
    /* Initialize HAL state variables. */
#ifdef COMPASS_ENABLED
    hal.sensors = ACCEL_ON | GYRO_ON | COMPASS_ON;
#else
    hal.sensors = ACCEL_ON | GYRO_ON;
#endif
    hal.dmp_on = 0;
    hal.report = 0;
    hal.rx.cmd = 0;
    hal.next_pedo_ms = 0;
    hal.next_compass_ms = 0;
    hal.next_temp_ms = 0;

  /* Compass reads are handled by scheduler. */
  timestamp = get_tick_count();

    /* To initialize the DMP:
     * 1. Call dmp_load_motion_driver_firmware(). This pushes the DMP image in
     *    inv_mpu_dmp_motion_driver.h into the MPU memory.
     * 2. Push the gyro and accel orientation matrix to the DMP.
     * 3. Register gesture callbacks. Don't worry, these callbacks won't be
     *    executed unless the corresponding feature is enabled.
     * 4. Call dmp_enable_feature(mask) to enable different features.
     * 5. Call dmp_set_fifo_rate(freq) to select a DMP output rate.
     * 6. Call any feature-specific control functions.
     *
     * To enable the DMP, just call mpu_set_dmp_state(1). This function can
     * be called repeatedly to enable and disable the DMP at runtime.
     *
     * The following is a short summary of the features supported in the DMP
     * image provided in inv_mpu_dmp_motion_driver.c:
     * DMP_FEATURE_LP_QUAT: Generate a gyro-only quaternion on the DMP at
     * 200Hz. Integrating the gyro data at higher rates reduces numerical
     * errors (compared to integration on the MCU at a lower sampling rate).
     * DMP_FEATURE_6X_LP_QUAT: Generate a gyro/accel quaternion on the DMP at
     * 200Hz. Cannot be used in combination with DMP_FEATURE_LP_QUAT.
     * DMP_FEATURE_TAP: Detect taps along the X, Y, and Z axes.
     * DMP_FEATURE_ANDROID_ORIENT: Google's screen rotation algorithm. Triggers
     * an event at the four orientations where the screen should rotate.
     * DMP_FEATURE_GYRO_CAL: Calibrates the gyro data after eight seconds of
     * no motion.
     * DMP_FEATURE_SEND_RAW_ACCEL: Add raw accelerometer data to the FIFO.
     * DMP_FEATURE_SEND_RAW_GYRO: Add raw gyro data to the FIFO.
     * DMP_FEATURE_SEND_CAL_GYRO: Add calibrated gyro data to the FIFO. Cannot
     * be used in combination with DMP_FEATURE_SEND_RAW_GYRO.
     */
    dmp_load_motion_driver_firmware();
    dmp_set_orientation(
        inv_orientation_matrix_to_scalar(gyro_pdata.orientation));
    dmp_register_tap_cb(tap_cb);
    dmp_register_android_orient_cb(android_orient_cb);
    /*
     * Known Bug -
     * DMP when enabled will sample sensor data at 200Hz and output to FIFO at the rate
     * specified in the dmp_set_fifo_rate API. The DMP will then sent an interrupt once
     * a sample has been put into the FIFO. Therefore if the dmp_set_fifo_rate is at 25Hz
     * there will be a 25Hz interrupt from the MPU device.
     *
     * There is a known issue in which if you do not enable DMP_FEATURE_TAP
     * then the interrupts will be at 200Hz even if fifo rate
     * is set at a different rate. To avoid this issue include the DMP_FEATURE_TAP
     *
     * DMP sensor fusion works only with gyro at +-2000dps and accel +-2G
     */
    hal.dmp_features = DMP_FEATURE_6X_LP_QUAT | DMP_FEATURE_TAP |
        DMP_FEATURE_ANDROID_ORIENT | DMP_FEATURE_SEND_RAW_ACCEL | DMP_FEATURE_SEND_CAL_GYRO |
        DMP_FEATURE_GYRO_CAL;
    dmp_enable_feature(hal.dmp_features);
    dmp_set_fifo_rate(DEFAULT_MPU_HZ);
    mpu_set_dmp_state(1);
    hal.dmp_on = 1;

  while(1){
    
    unsigned long sensor_timestamp;
    int new_data = 0;
    if (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_RXNE)) {
        /* A byte has been received via USART. See handle_input for a list of
         * valid commands.
         */
        USART_ClearFlag(DEBUG_USART, USART_FLAG_RXNE);
        handle_input();
    }
    timestamp = get_tick_count();

#ifdef COMPASS_ENABLED
        /* We're not using a data ready interrupt for the compass, so we'll
         * make our compass reads timer-based instead.
         */
        if ((timestamp > hal.next_compass_ms) && !hal.lp_accel_mode &&
            hal.new_gyro && (hal.sensors & COMPASS_ON)) {
            hal.next_compass_ms = timestamp + COMPASS_READ_MS;
            new_compass = 1;
        }
#endif
        /* Temperature data doesn't need to be read with every gyro sample.
         * Let's make them timer-based like the compass reads.
         */
        if (timestamp > hal.next_temp_ms) {
            hal.next_temp_ms = timestamp + TEMP_READ_MS;
            new_temp = 1;
        }

    if (hal.motion_int_mode) {
        /* Enable motion interrupt. */
        mpu_lp_motion_interrupt(500, 1, 5);
        /* Notify the MPL that contiguity was broken. */
        inv_accel_was_turned_off();
        inv_gyro_was_turned_off();
        inv_compass_was_turned_off();
        inv_quaternion_sensor_was_turned_off();
        /* Wait for the MPU interrupt. */
        while (!hal.new_gyro) {}
        /* Restore the previous sensor configuration. */
        mpu_lp_motion_interrupt(0, 0, 0);
        hal.motion_int_mode = 0;
    }

    if (!hal.sensors || !hal.new_gyro) {
        continue;
    }    

        if (hal.new_gyro && hal.lp_accel_mode) {
            short accel_short[3];
            long accel[3];
            mpu_get_accel_reg(accel_short, &sensor_timestamp);
            accel[0] = (long)accel_short[0];
            accel[1] = (long)accel_short[1];
            accel[2] = (long)accel_short[2];
            inv_build_accel(accel, 0, sensor_timestamp);
            new_data = 1;
            hal.new_gyro = 0;
        } else if (hal.new_gyro && hal.dmp_on) {
            short gyro[3], accel_short[3], sensors;
            unsigned char more;
            long accel[3], quat[4], temperature;
            /* This function gets new data from the FIFO when the DMP is in
             * use. The FIFO can contain any combination of gyro, accel,
             * quaternion, and gesture data. The sensors parameter tells the
             * caller which data fields were actually populated with new data.
             * For example, if sensors == (INV_XYZ_GYRO | INV_WXYZ_QUAT), then
             * the FIFO isn't being filled with accel data.
             * The driver parses the gesture data to determine if a gesture
             * event has occurred; on an event, the application will be notified
             * via a callback (assuming that a callback function was properly
             * registered). The more parameter is non-zero if there are
             * leftover packets in the FIFO.
             */
            dmp_read_fifo(gyro, accel_short, quat, &sensor_timestamp, &sensors, &more);
            if (!more)
                hal.new_gyro = 0;
            if (sensors & INV_XYZ_GYRO) {
                /* Push the new data to the MPL. */
                inv_build_gyro(gyro, sensor_timestamp);
                new_data = 1;
                if (new_temp) {
                    new_temp = 0;
                    /* Temperature only used for gyro temp comp. */
                    mpu_get_temperature(&temperature, &sensor_timestamp);
                    inv_build_temp(temperature, sensor_timestamp);
                }
            }
            if (sensors & INV_XYZ_ACCEL) {
                accel[0] = (long)accel_short[0];
                accel[1] = (long)accel_short[1];
                accel[2] = (long)accel_short[2];
                inv_build_accel(accel, 0, sensor_timestamp);
                new_data = 1;
            }
            if (sensors & INV_WXYZ_QUAT) {
                inv_build_quat(quat, 0, sensor_timestamp);
                new_data = 1;
            }
        } else if (hal.new_gyro) {
            short gyro[3], accel_short[3];
            unsigned char sensors, more;
            long accel[3], temperature;
            /* This function gets new data from the FIFO. The FIFO can contain
             * gyro, accel, both, or neither. The sensors parameter tells the
             * caller which data fields were actually populated with new data.
             * For example, if sensors == INV_XYZ_GYRO, then the FIFO isn't
             * being filled with accel data. The more parameter is non-zero if
             * there are leftover packets in the FIFO. The HAL can use this
             * information to increase the frequency at which this function is
             * called.
             */
            hal.new_gyro = 0;
            mpu_read_fifo(gyro, accel_short, &sensor_timestamp,
                &sensors, &more);
            if (more)
                hal.new_gyro = 1;
            if (sensors & INV_XYZ_GYRO) {
                /* Push the new data to the MPL. */
                inv_build_gyro(gyro, sensor_timestamp);
                new_data = 1;
                if (new_temp) {
                    new_temp = 0;
                    /* Temperature only used for gyro temp comp. */
                    mpu_get_temperature(&temperature, &sensor_timestamp);
                    inv_build_temp(temperature, sensor_timestamp);
                }
            }
            if (sensors & INV_XYZ_ACCEL) {
                accel[0] = (long)accel_short[0];
                accel[1] = (long)accel_short[1];
                accel[2] = (long)accel_short[2];
                inv_build_accel(accel, 0, sensor_timestamp);
                new_data = 1;
            }
        }
#ifdef COMPASS_ENABLED
        if (new_compass) {
            short compass_short[3];
            long compass[3];
            new_compass = 0;
            /* For any MPU device with an AKM on the auxiliary I2C bus, the raw
             * magnetometer registers are copied to special gyro registers.
             */
            if (!mpu_get_compass_reg(compass_short, &sensor_timestamp)) {
                compass[0] = (long)compass_short[0];
                compass[1] = (long)compass_short[1];
                compass[2] = (long)compass_short[2];
                /* NOTE: If using a third-party compass calibration library,
                 * pass in the compass data in uT * 2^16 and set the second
                 * parameter to INV_CALIBRATED | acc, where acc is the
                 * accuracy from 0 to 3.
                 */
                inv_build_compass(compass, 0, sensor_timestamp);
            }
            new_data = 1;
        }
#endif
        if (new_data) {
            inv_execute_on_data();
            /* This function reads bias-compensated sensor data and sensor
             * fusion outputs from the MPL. The outputs are formatted as seen
             * in eMPL_outputs.c. This function only needs to be called at the
             * rate requested by the host.
             */
            read_from_mpl();
        }
    }
}

static void	X_MeterPointer(HDC hdc,int cx,int cy,int r,u32 color,int st_angle,int angle_size,int dat_size,int dat_val,int style)
{
	int angle; 
	POINT pt[8];

	////

	if(angle_size>360)	angle_size=360;

	angle = st_angle+(dat_val*angle_size)/dat_size;
	angle = MIN(angle,st_angle+angle_size);

	angle -= 90;

//	GUI_DEBUG("%d", angle);
  
  if(style==0)
  {
    pt[0].x	 = (int)(cx + sin(angle*3.14/180)*r);
    pt[0].y	 = (int)(cy - cos(angle*3.14/180)*r);

    pt[1].x = (int)(cx + sin((angle+90)*3.14/180)*(r>>3));
    pt[1].y = (int)(cy - cos((angle+90)*3.14/180)*(r>>3));

    pt[2].x = (int)(cx + sin((angle+180)*3.14/180)*(r));
    pt[2].y = (int)(cy - cos((angle+180)*3.14/180)*(r));

    pt[3].x = (int)(cx + sin((angle+270)*3.14/180)*(r>>3));//>>4
    pt[3].y = (int)(cy - cos((angle+270)*3.14/180)*(r>>3));//>>4

    pt[4].x = pt[0].x;
    pt[4].y = pt[0].y;


    SetBrushColor(hdc,color);
    EnableAntiAlias(hdc, TRUE);
    FillPolygon(hdc,0,0,pt,5);
    EnableAntiAlias(hdc, FALSE);
    //SetPenColor(hdc,color);
    //AA_DrawPolygon(hdc,0,0,pt,5); //?????????
  }


  if(style==1)
  {
    pt[0].x	 = (int)(cx + sin(angle*3.14/180)*r);
    pt[0].y	 = (int)(cy - cos(angle*3.14/180)*r);

    pt[1].x = (int)(cx + sin((angle+4)*3.14/180)*(r-(r>>2)));
    pt[1].y = (int)(cy - cos((angle+4)*3.14/180)*(r-(r>>2)));

    pt[2].x = cx;
    pt[2].y = cy;

    pt[3].x = (int)(cx + sin((angle-4)*3.14/180)*(r-(r>>2)));
    pt[3].y = (int)(cy - cos((angle-4)*3.14/180)*(r-(r>>2)));

    pt[4].x = pt[0].x;
    pt[4].y = pt[0].y;


    FillPolygon(hdc,0,0,pt,5); //?????????

  }


  if(style==2)
  {
    POINT pt[7];

    pt[0].x	 = (int)(cx + sin(angle*3.14/180)*r);
    pt[0].y	 = (int)(cy - cos(angle*3.14/180)*r);

    pt[1].x = (int)(cx + sin((angle+6)*3.14/180)*(r-(r>>2)));
    pt[1].y = (int)(cy - cos((angle+6)*3.14/180)*(r-(r>>2)));

    pt[2].x = (int)(cx + sin((angle+1)*3.14/180)*(r-(r>>2)+(r>>4)));
    pt[2].y = (int)(cy - cos((angle+1)*3.14/180)*(r-(r>>2)+(r>>4)));

    pt[3].x = (int)cx;
    pt[3].y = (int)cy;

    pt[4].x = (int)(cx + sin((angle-1)*3.14/180)*(r-(r>>2)+(r>>4)));
    pt[4].y = (int)(cy - cos((angle-1)*3.14/180)*(r-(r>>2)+(r>>4)));

    pt[5].x = (int)(cx + sin((angle-6)*3.14/180)*(r-(r>>2)));
    pt[5].y = (int)(cy - cos((angle-6)*3.14/180)*(r-(r>>2)));

    pt[6].x = pt[0].x;
    pt[6].y = pt[0].y;

    FillPolygon(hdc,0,0,pt,7);

  }

  if(style==3) 
  {
    POINT pt[7];
    int x1,y1;

    x1 = (int)(cx + sin((angle-0)*3.14/180)*(r-(r>>5)));
    y1 = (int)(cy - cos((angle-0)*3.14/180)*(r-(r>>5)));

    pt[0].x	 = (int)(x1 + sin((angle-90)*3.14/180)*(2));
    pt[0].y	 = (int)(y1 - cos((angle-90)*3.14/180)*(2));

    pt[1].x	 = x1;
    pt[1].y	 = y1;

    pt[2].x	 = (int)(x1 + sin((angle+90)*3.14/180)*(2));
    pt[2].y	 = (int)(y1 - cos((angle+90)*3.14/180)*(2));


    pt[3].x	 = (int)(cx + sin((angle+90)*3.14/180)*(2));
    pt[3].y	 = (int)(cy - cos((angle+90)*3.14/180)*(2));

    pt[4].x	 = cx;
    pt[4].y	 = cy;

    pt[5].x	 = (int)(cx + sin((angle-90)*3.14/180)*(2));
    pt[5].y	 = (int)(cy - cos((angle-90)*3.14/180)*(2));

    pt[6].x = pt[0].x;
    pt[6].y = pt[0].y;

    FillPolygon(hdc,0,0,pt,7);

  }

  if(style==4) 
  {
    POINT pt[7];
    int x1,y1;

    x1 = (int)(cx + sin((angle-0)*3.14/180)*(r-(r>>5)));
    y1 = (int)(cy - cos((angle-0)*3.14/180)*(r-(r>>5)));

    pt[0].x	 = (int)(cx + sin((angle-60)*3.14/180)*(5));
    pt[0].y	 = (int)(cy - cos((angle-60)*3.14/180)*(5));

    pt[1].x	 = cx;
    pt[1].y	 = cy;

    pt[2].x	 = (int)(cx + sin((angle+60)*3.14/180)*(5));
    pt[2].y	 = (int)(cy - cos((angle+60)*3.14/180)*(5));


    pt[3].x	 = (int)(pt[2].x + sin((angle+0)*3.14/180)*(r-(r>>2)));
    pt[3].y	 = (int)(pt[2].y - cos((angle+0)*3.14/180)*(r-(r>>2)));

    pt[4].x	 = x1;
    pt[4].y	 = y1;

    pt[5].x	 = (int)(pt[0].x + sin((angle+0)*3.14/180)*(r-(r>>2)));
    pt[5].y	 = (int)(pt[0].y - cos((angle+0)*3.14/180)*(r-(r>>2)));

    pt[6].x = pt[0].x;
    pt[6].y = pt[0].y;

    FillPolygon(hdc,0,0,pt,7);

  }

  if(style==5) 
  {
    POINT pt[5];
    int x1,y1;

    x1 = (int)(cx + sin((angle-0)*3.14/180)*(r-(r>>5)));
    y1 = (int)(cy - cos((angle-0)*3.14/180)*(r-(r>>5)));

    pt[0].x	 = x1;
    pt[0].y	 = y1;

    pt[1].x = (int)(cx + sin((angle+12)*3.14/180)*(r-(r>>1)));
    pt[1].y = (int)(cy - cos((angle+12)*3.14/180)*(r-(r>>1)));

    pt[2].x = (int)cx;
    pt[2].y = (int)cy;

    pt[3].x = (int)(cx + sin((angle-12)*3.14/180)*(r-(r>>1)));
    pt[3].y = (int)(cy - cos((angle-12)*3.14/180)*(r-(r>>1)));

    pt[4].x = pt[0].x;
    pt[4].y = pt[0].y;

    FillPolygon(hdc,0,0,pt,5);

  }

  if(style==6) 
  {
    POINT pt[5];
    int x1,y1;

    x1 = (int)(cx + sin((angle-0)*3.14/180)*((r*15)>>4));
    y1 = (int)(cy - cos((angle-0)*3.14/180)*((r*15)>>4));

    pt[0].x	 = x1;
    pt[0].y	 = y1;

    pt[1].x = (int)(cx + sin((angle+15)*3.14/180)*((r*4)>>4));
    pt[1].y = (int)(cy - cos((angle+15)*3.14/180)*((r*4)>>4));

    pt[2].x = (int)cx;
    pt[2].y = (int)cy;

    pt[3].x = (int)(cx + sin((angle-15)*3.14/180)*((r*4)>>4));
    pt[3].y = (int)(cy - cos((angle-15)*3.14/180)*((r*4)>>4));

    pt[4].x = pt[0].x;
    pt[4].y = pt[0].y;

    FillPolygon(hdc,0,0,pt,5);

  }

	////
#if 0	
	SetBrushColor(hdc,fr_color);
	FillCircle(hdc,cx,cy,r>>3);
#endif

}

//退出按钮重绘制
static void CollectVoltage_ExitButton_OwnerDraw(DRAWITEM_HDR *ds)
{
  HDC hdc;
  RECT rc;
//  HWND hwnd;

	hdc = ds->hDC;   
	rc = ds->rc; 
//  hwnd = ds->hwnd;

//  GetClientRect(hwnd, &rc_tmp);//得到控件的位置
//  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//坐标转换

//  BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_bk, rc_tmp.x, rc_tmp.y, SRCCOPY);

  if (ds->State & BST_PUSHED)
	{ //按钮是按下状态
		SetPenColor(hdc, MapRGB(hdc, 1, 191, 255));
	}
	else
	{ //按钮是弹起状态

		SetPenColor(hdc, MapRGB(hdc, 250, 250, 250));      //设置画笔色
	}

  SetPenSize(hdc, 2);

  InflateRect(&rc, 0, -1);
  
  for(int i=0; i<4; i++)
  {
    HLine(hdc, rc.x, rc.y, rc.w);
    rc.y += 9;
  }
}

static LRESULT	win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)
  {
    case WM_CREATE:
    {
      RECT rc;
      GetClientRect(hwnd, &rc);

      xTaskCreate((TaskFunction_t )Gyro_Dispose_Task,      /* 任务入口函数 */
                  (const char*    )"Gyro Dispose Task",    /* 任务名字 */
                  (uint16_t       )3*1024/4,               /* 任务栈大小FreeRTOS的任务栈以字为单位 */
                  (void*          )NULL,                   /* 任务入口函数参数 */
                  (UBaseType_t    )5,                      /* 任务的优先级 */
                  (TaskHandle_t*  )&Gyro_Task_Handle);     /* 任务控制块指针 */
                      
      CreateWindow(BUTTON, L"O", WS_TRANSPARENT|BS_FLAT | BS_NOTIFY |WS_OWNERDRAW|WS_VISIBLE,
                  740, 12, 36, 36, hwnd, eID_Gyro_EXIT, NULL, NULL); 
      
      BOOL res;
      u8 *jpeg_buf;
      u32 jpeg_size;
      JPG_DEC *dec;
      res = RES_Load_Content(GUI_GYRO_BACKGROUNG_PIC, (char**)&jpeg_buf, &jpeg_size);
    //   res = FS_Load_Content(GUI_GYRO_BACKGROUNG_PIC, (char**)&jpeg_buf, &jpeg_size);
      bk_hdc = CreateMemoryDC(SURF_SCREEN, GUI_XSIZE, GUI_YSIZE);
      if(res)
      {
        /* 根据图片数据创建JPG_DEC句柄 */
        dec = JPG_Open(jpeg_buf, jpeg_size);

        /* 绘制至内存对象 */
        JPG_Draw(bk_hdc, 0, 0, dec);

        /* 关闭JPG_DEC句柄 */
        JPG_Close(dec);
      }
      /* 释放图片内容空间 */
      RES_Release_Content((char **)&jpeg_buf);

      u8 *pic_buf;
      u32 pic_size;
      
      #if BMP
      PNG_DEC *png_dec;
      BITMAP png_bm;
      #endif 
      
      /* 创建横滚角刻度 HDC */
      Roll_hdc = CreateMemoryDC((SURF_FORMAT)COLOR_FORMAT_ARGB8888, 43, 582);
      ClrDisplay(Roll_hdc, NULL, 0);
      res = RES_Load_Content(GUI_GYRO_ROLL_PIC, (char**)&pic_buf, &pic_size);
    //   res = FS_Load_Content(GUI_GYRO_ROLL_PIC, (char**)&pic_buf, &pic_size);
      if(res)
      {
        #if BMP
          png_dec = PNG_Open(pic_buf);
          PNG_GetBitmap(png_dec, &png_bm);
          DrawBitmap(Roll_hdc, 0,0, &png_bm, NULL);
          PNG_Close(png_dec);
        #else
          BMP_Draw(Roll_hdc, 0, 0, pic_buf, NULL);
        #endif     
      }
      /* 释放图片内容空间 */
      RES_Release_Content((char **)&pic_buf);

      /* 创建俯仰角刻度 HDC */
      Pitch_hdc = CreateMemoryDC((SURF_FORMAT)COLOR_FORMAT_ARGB8888, 1168, 36);
      ClrDisplay(Pitch_hdc, NULL, 0);
      res = RES_Load_Content(GUI_GYRO_PITCH_PIC, (char**)&pic_buf, &pic_size);
    //   res = FS_Load_Content(GUI_GYRO_PITCH_PIC, (char**)&pic_buf, &pic_size);
      if(res)
      {
        #if BMP
          png_dec = PNG_Open(pic_buf);
          PNG_GetBitmap(png_dec, &png_bm);
          DrawBitmap(Pitch_hdc, 0,0, &png_bm, NULL);
          PNG_Close(png_dec);
        #else
          BMP_Draw(Pitch_hdc, 0, 0, pic_buf, NULL);
        #endif     
      }
      /* 释放图片内容空间 */
      RES_Release_Content((char **)&pic_buf);

      break;
    } 
    case WM_TIMER:
    {
      int tmr_id;

      tmr_id = wParam;    // 定时器 ID

      if (tmr_id == 10)    
      {
        
      }
      
      break;
    }

    case WM_ERASEBKGND:
    {
      HDC hdc =(HDC)wParam;
//      uint32_t tick;
//      
//      tick = GUI_GetTickCount();

      BitBlt(hdc, 0, 0, GUI_XSIZE, GUI_YSIZE, bk_hdc, 0, 0, SRCCOPY);
      //GUI_DEBUG("刷背景耗时：%d", GUI_GetTickCount() - tick);
      return FALSE;
    }

    case WM_PAINT:
    {
      HDC hdc;
      HDC hdc_pointer, hdc_pitch_temp, hdc_roll_temp;
      PAINTSTRUCT ps;
      int Copy_Start = 0;    // 拷贝刻度的起始位置
      int Copy_Len   = 0;    // 要拷贝的长度
      int Copy_Site  = 0;    // 要拷贝的位置

      RECT rc =  {0, 0, GUI_XSIZE, GUI_YSIZE};
      hdc_pointer    = CreateMemoryDC(SURF_SCREEN, COMPASS_W, COMPASS_H);
      hdc_pitch_temp = CreateMemoryDC(SURF_SCREEN, 320, 60);
      hdc_roll_temp  = CreateMemoryDC(SURF_SCREEN, 60, 320);
      hdc = BeginPaint(hwnd, &ps);

      /* 偏航角 */
      BitBlt(hdc_pointer, 0, 0, COMPASS_W, COMPASS_H, bk_hdc, 135, 125, SRCCOPY);
      X_MeterPointer(hdc_pointer, COMPASS_W/2, COMPASS_H/2, COMPASS_W/2-1, MapRGB(hdc_pointer,227,227,227), 90, 180, 180, Yaw, 0);
      BitBlt(hdc, 135, 123, COMPASS_W, COMPASS_H, hdc_pointer, 0, 0, SRCCOPY);

      /********************************** 俯仰角 ***************************/
      BitBlt(hdc_pitch_temp, 0, 0, 320, 60, bk_hdc, 449, 394, SRCCOPY);

      /* 计算要拷贝的刻度位置 */
      Copy_Start = 434 + Pitch * 2;    

      /* 拷贝不超过边界 */
      Copy_Site = 13;
      Copy_Len  = 300;

      /* 拷贝区域越界处理 */
      if (Copy_Start < 0)
      {
          Copy_Site -= Copy_Start;    // 右移拷贝的位置
          Copy_Len  += Copy_Start;    // 减少拷贝的长度
          Copy_Start  = 0;            // 从头开始拷贝
      }
      else if (Copy_Start + 300 > 1168)    // 要拷贝的区域超过了图片    1168 ：刻度的长度
      {
          Copy_Len = 1168 - Copy_Start;    // 限制拷贝长度
      }
      
      BitBlt(hdc_pitch_temp, Copy_Site, 9, Copy_Len, 36, Pitch_hdc, Copy_Start, 0, SRCCOPY);
      BitBlt(hdc, 449, 394, 300, 60, hdc_pitch_temp, 0, 0, SRCCOPY);

      /******************************* 横滚角 ********************************/
      BitBlt(hdc_roll_temp, 0, 0, 60, 320, bk_hdc, 570, 63, SRCCOPY);
      
      /* 计算要拷贝的刻度位置 */
      Copy_Start = 142 - Roll * 3;    

      /* 拷贝不超过边界 */
      Copy_Site = 13;
      Copy_Len  = 300;

      /* 拷贝区域越界处理 */
      if (Copy_Start < 0)
      {
          Copy_Site -= Copy_Start;    // 下移拷贝的位置
          Copy_Len  += Copy_Start;    // 减少拷贝的长度
          Copy_Start  = 0;            // 从头开始拷贝
      }
      else if (Copy_Start + 300 > 582)    // 要拷贝的区域超过了图片
      {
          Copy_Len = 582 - Copy_Start;    // 限制拷贝长度
      }

      BitBlt(hdc_roll_temp, 9, Copy_Site, 43, Copy_Len, Roll_hdc, 0, Copy_Start, SRCCOPY);    // 拷贝刻度到缓冲区
      BitBlt(hdc, 570, 63, 60, 320, hdc_roll_temp, 0, 0, SRCCOPY);

      EndPaint(hwnd, &ps);
      DeleteDC(hdc_pointer);
      DeleteDC(hdc_pitch_temp);
      DeleteDC(hdc_roll_temp);

      break;
    }

    case WM_DRAWITEM:
    {
       DRAWITEM_HDR *ds;
       ds = (DRAWITEM_HDR*)lParam;
       switch(ds->ID)
       {
          case eID_Gyro_EXIT:
          {
            CollectVoltage_ExitButton_OwnerDraw(ds);
            return TRUE;             
          }  
       }

       break;
    }
    case WM_NOTIFY:
    {
      u16 code, id;
      id  =LOWORD(wParam);//获取消息的ID码
      code=HIWORD(wParam);//获取消息的类型    
      if(code == BN_CLICKED && id == eID_Gyro_EXIT)    // 退出按钮按下
      { 
        PostCloseMessage(hwnd);    // 发送关闭窗口的消息
        break;
      }
      
      break;
    } 

    case WM_DESTROY:
    { 
      DeleteDC(bk_hdc);
      DeleteDC(Roll_hdc);
      DeleteDC(Pitch_hdc);
      EXTI_MPU_Disable();                     // 禁用外部中断
      GUI_Thread_Delete(Gyro_Task_Handle);    // 删除网络处理任务
      
      return PostQuitMessage(hwnd);	
    } 

    default:
      return	DefWindowProc(hwnd, msg, wParam, lParam);   
  }
  
  return WM_NULL;
  
}

void GUI_Gyro_Dialog(void)
{
	
	WNDCLASS	wcex;
	MSG msg;
  
	wcex.Tag = WNDCLASS_TAG;
	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = win_proc; //设置主窗口消息处理的回调函数.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);
   
	//创建主窗口
	Gyro_Main_Handle = CreateWindowEx(WS_EX_NOFOCUS|WS_EX_FRAMEBUFFER,
                              &wcex,
                              L"GUI Gyro Dialog",
                              WS_VISIBLE|WS_CLIPCHILDREN,
                              0, 0, GUI_XSIZE, GUI_YSIZE,
                              NULL, NULL, NULL, NULL);

   //显示主窗口
	ShowWindow(Gyro_Main_Handle, SW_SHOW);

	//开始窗口消息循环(窗口关闭并销毁时,GetMessage将返回FALSE,退出本消息循环)。
	while (GetMessage(&msg, Gyro_Main_Handle))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}


