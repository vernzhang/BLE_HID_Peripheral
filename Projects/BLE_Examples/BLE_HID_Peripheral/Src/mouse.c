/******************** (C) COPYRIGHT 2019 STMicroelectronics ********************
* File Name          : mouse.c
* Author             : AMS - RF Application Team
* Version            : V1.0.0
* Date               : 05-June-2020
* Description        : BlueNRG-LP mouse init and process data functions
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "bluenrg_lp_it.h"
#include "ble_const.h"
#include "bluenrg_lp_stack.h"
#include "rf_driver_hal_power_manager.h"
#include "rf_driver_hal_vtimer.h"
#include "bluenrg_lp_evb_com.h"
#include "hid_peripheral.h"
#include "hid_peripheral_config.h"
#include "bluenrg_lp_evb_config.h" 
#include "clock.h" 
#include "rf_driver_ll_flash.h"

#include "mouse.h"

extern void Delay_10MS(uint32_t count);

/* Private typedef -----------------------------------------------------------*/
uint8_t deviceStatus = 0;

/* Private define ------------------------------------------------------------*/

#define IDLE_CONNECTION_TIMEOUT (300*1000) // 5 min
uint32_t poweroff_count = IDLE_CONNECTION_TIMEOUT/10;

#define WAKEUP_NOTIFY        				10 // 10 ms

#define BUTTON_LEFT_PRESSED        1<<0
#define BUTTON_RIGHT_PRESSED       1<<1
#define BUTTON_STATUS_CHANGED_MASK 1<<2

#define NUM_REPORTS 1

/* Private macro -------------------------------------------------------------*/
#define DEBUG 1
#ifdef DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

/* Private variables ---------------------------------------------------------*/

hidService_Type hid_param;
report_Type reportReferenceDesc[NUM_REPORTS];

uint8_t dev_name[]={'S', 'T', 'M', 'o', 'u', 's', 'e'}; 

// Mouse report descriptor
uint8_t reportDesc[] = {
  0x05, 0x01,        //USAGE_PAGE(Generic Desktop)
  0x09, 0x02,        //USAGE(Mouse)
  0xA1, 0x01,        //COLLECTION(Application)
  0x85, REPORT_ID,   // REPORT ID
  0x09, 0x01,        //USAGE(Pointer)
  0xA1, 0x00,        //COLLECTION(Physical)
  0x05, 0x09,        //USAGE_PAGE(Button)
  0x19, 0x01,        //USAGE_MINIMUM(Button 1)
  0x29, 0x03,        //USAGE_MAXIMUM(Button 3)
  0x15, 0x00,        //LOGICAL_MINIMUM(0)
  0x25, 0x01,        //LOGICAL_MAXIMUM(1)
  0x75, 0x01,        //REPORT_SIZE(1)
  0x95, 0x03,        //REPORT_COUNT(3)
  0x81, 0x02,        //INPUT(Data, Variable, Absolute) ; Button states
  0x75, 0x05,        //REPORT_SIZE(5)
  0x95, 0x01,        //REPORT_COUNT(1)
  0x81, 0x01,        //INPUT(Constant, Variable, Absolute) ; Reserved bits
  0x05, 0x01,        //USAGE_PAGE(Generic Desktop) 
  0x09, 0x30,        //USAGE(X)
  0x09, 0x31,        //USAGE(Y)
  0x16, 0x01, 0xF8,  //LOGICAL_MINIMUM(-2047)
  0x26, 0xFF, 0x07,  //LOGICAL_MAXIMUM(2047)
  0x75, 0x0C,        //REPORT_SIZE(12 bits)
  0x95, 0x02,        //REPORT_COUNT(2)
  0x81, 0x06,        //INPUT(Data, Variable, Relative) ; X & Y
  0x09, 0x38,        //USAGE(Z)
  0x15, 0x81,        //LOGICAL_MINIMUM(-127)
  0x25, 0x7F,        //LOGICAL_MAXIMUM(127)
  0x75, 0x08,        //REPORT_SIZE(8)
  0x95, 0x01,        //REPORT_COUNT(1)
  0x81, 0x06,        //INPUT(Data, Variable, Relative) ; Z
  0xC0,              //END_COLLECTION
  0xC0               //END_COLLECTION
};

uint8_t leftButtonPressed;

static void sensorInit(void)
{
  /* Button Init */
  BSP_PB_Init(BSP_SLIDEUP, BUTTON_MODE_GPIO);
}

/* Public Functions ----------------------------------------------------------*/

void Sensor_IrqHandler(void)
{
 if(BSP_PB_GetITPendingBit(BSP_SLIDEUP) == SET) 
  {
    BSP_PB_ClearITPendingBit(BSP_SLIDEUP);
    leftButtonPressed = TRUE;
    hidSetNotificationPending(TRUE);
  }
}

void HAL_PWR_MNGR_WakeupIOCallback(uint32_t source)
{
  if(source & BSP_SLIDEUP_WAKEUP){   
    leftButtonPressed = TRUE;
    hidSetNotificationPending(TRUE);
  }
}

void setDefaultHidParams(void)
{
  hid_param.bootSupport = FALSE;
  hid_param.reportSupport = TRUE;
  hid_param.num_reports = NUM_REPORTS;
  hid_param.reportReferenceDesc = reportReferenceDesc;
  hid_param.reportReferenceDesc[0].ID = REPORT_ID;
  hid_param.reportReferenceDesc[0].type = INPUT_REPORT;
  hid_param.isBootDevKeyboard = FALSE;
  hid_param.isBootDevMouse = FALSE;
  hid_param.externalReportEnabled = 0;
  hid_param.includedServiceEnabled = FALSE;
  hid_param.informationCharac[0] = 0x01;
  hid_param.informationCharac[1] = 0x01;
  hid_param.informationCharac[2] = 0;
  hid_param.informationCharac[3] = 0x01;
}

void setTestHidParams(void)
{
  hid_param.bootSupport = TRUE; 
  hid_param.reportSupport = TRUE;
  hid_param.reportReferenceDesc[0].ID = REPORT_ID;
  hid_param.isBootDevKeyboard = FALSE;
  hid_param.isBootDevMouse = TRUE; 
  hid_param.externalReportEnabled = 1;  
  hid_param.includedServiceEnabled = FALSE;
  hid_param.informationCharac[0] = 0x01;
  hid_param.informationCharac[1] = 0x01;
  hid_param.informationCharac[2] = 0;
  hid_param.informationCharac[3] = 0x01;
}

void setHidParams_ReportType(uint8_t reportType)
{
  hid_param.reportReferenceDesc[0].type = reportType;
} 

uint8_t Configure_HidPeripheral(void)
{
  uint8_t ret;
  batteryService_Type battery;
  devInfService_Type devInf;
  hidService_Type hid;
  connParam_Type connParam;

  /* HID Peripheral Init */
  connParam.interval_min = 6;
  connParam.interval_max = 12;
  connParam.slave_latency = 0;
  connParam.timeout_multiplier = 300;
  ret = hidDevice_Init(IO_CAP_NO_INPUT_NO_OUTPUT /*IO_CAP_DISPLAY_ONLY*/, connParam, sizeof(dev_name), dev_name, STATIC_RANDOM_ADDR);
  if (ret != BLE_STATUS_SUCCESS) {
    PRINTF("Error in hidDevice_Init() 0x%02x\n", ret);
    return ret;
  }
  /* Set the HID Peripheral Security */
  ret = hidSetDeviceSecurty(FALSE, FALSE, 123456);
  if (ret != BLE_STATUS_SUCCESS) {
    PRINTF("Error in hidSetDeviceSecurty() 0x%02x\n", ret);
    return ret;
  }

  /* Set the HID Idle Timeout */
  hidSetIdleTimeout(IDLE_CONNECTION_TIMEOUT);
  
  /* Set the TX Power */
  ret = aci_hal_set_tx_power_level(0, 24);
  if (ret != BLE_STATUS_SUCCESS) {
    PRINTF("Error with aci_hal_set_tx_power_level() 0x%02x\n", ret);
    return ret;
  }

  /**** Setup the GATT Database ****/
  /* Battery Service */
  battery.inReportMap = FALSE;
  /* Device Information Service */
  memcpy(devInf.manufacName, "ST Micro", 8);
  memcpy(devInf.modelNumber, "0001", 4);
  memcpy(devInf.fwRevision, "0630", 4);
  memcpy(devInf.swRevision, "0001", 4);
  devInf.pnpID[0] = 0x01;
  devInf.pnpID[1] = 0x30;
  devInf.pnpID[2] = 0x00;
  devInf.pnpID[3] = 0xfc;
  devInf.pnpID[4] = 0x00;
  devInf.pnpID[5] = 0xec;
  devInf.pnpID[6] = 0x00;
 
  /* HID Service */
  hid = hid_param;  
  hid.reportDescLen = sizeof(reportDesc);
  hid.reportDesc = reportDesc;

  ret = hidAddServices(&battery, &devInf, &hid);
  if (ret != BLE_STATUS_SUCCESS) {
    PRINTF("Error in hidAddServices() 0x%02x\n", ret);
    return ret;
  }

  /* Init the Acc + Gyro sensor */
  leftButtonPressed = FALSE;
  sensorInit();
  
  /* Set the HID Peripheral device discoverable */
  ret = hidSetDeviceDiscoverable(LIMITED_DISCOVERABLE_MODE, sizeof(dev_name), dev_name);
  if (ret != BLE_STATUS_SUCCESS) {
    PRINTF("Error in hidSetDeviceDiscoverable() 0x%02x\n", ret);
    return ret;
  }
  
  PRINTF("HID Mouse Configured\n");

  return BLE_STATUS_SUCCESS;
}

void DevicePowerSaveProcedure(void)
{
  WakeupSourceConfig_TypeDef wakeupIO;
  PowerSaveLevels stopLevel;
  uint8_t hid_status = hidDeviceStatus();

	if (poweroff_count)
	{
		poweroff_count--;

		if (poweroff_count == 0)
		{		
			BSP_LED_Off(BSP_LED1);
			BSP_LED_Off(BSP_LED2);
			
	    /* No Wakeup Source needed */
	    wakeupIO.IO_Mask_High_polarity = 0;
	    wakeupIO.IO_Mask_Low_polarity = 0;
	    wakeupIO.RTC_enable = 0;
	    wakeupIO.LPU_enable = 0;
	    wakeupIO.IO_Mask_High_polarity = BSP_SLIDEUP_WAKEUP;
	    
	    /* Power Save Request */
	    HAL_PWR_MNGR_Request(POWER_SAVE_LEVEL_STOP_NOTIMER, wakeupIO, &stopLevel);
	    poweroff_count = IDLE_CONNECTION_TIMEOUT/10;

	    HAL_PWR_MNGR_ClearWakeupSource(BSP_SLIDEUP_WAKEUP);	    
	  }
  }
}

typedef struct {
  uint8_t buttons;    // bit0=左键, bit1=右键, bit2=中键
  int16_t x;           // X轴位移
  int16_t y;           // Y轴位移
} MouseReport;

void Send_Mouse_Report(MouseReport *report)
{
  uint8_t buff[5], ret;
  buff[0] = report->buttons;

  if (deviceStatus & HID_DEVICE_READY_TO_NOTIFY)
  {  	
		HOST_TO_LE_32(&buff[1], ((uint32_t)(report->x&0x0FFF)|(uint32_t)(report->y&0x0FFF)<<12));
		ret = hidSendReport(REPORT_ID, INPUT_REPORT, sizeof(buff), buff);
		
  	if (ret == BLE_STATUS_SUCCESS)
			hidSetNotificationPending(TRUE);
  	else if (ret != BLE_STATUS_INSUFFICIENT_RESOURCES)
  		PRINTF("Error while sending the hid report. (error = 0x%02x)\n", ret) ;
  }
}

// 抖音上滑（下一个视频）
void Douyin_Slide_Up(void)
{
  MouseReport report = {0};
  
  poweroff_count = IDLE_CONNECTION_TIMEOUT/10;
  
  // 1. 左键按下
  report.buttons = BUTTON_LEFT_PRESSED;
  report.x = 0;
  report.y = 0;
  Send_Mouse_Report(&report);
	
  // 2. 连续向上滑动（Y=-60，适配抖音阈值）
  report.y = -90;
  Send_Mouse_Report(&report);

  // 3. 左键释放
  report.buttons = 0;
  report.x = 0;
  report.y = 0;
	Send_Mouse_Report(&report);

  // 4. 核心新增：光标归位（发送反向位移，无按键）
  report.y = 30; // 向下移回原位
  Send_Mouse_Report(&report);
}

// 抖音下滑（上一个视频）
void Douyin_Slide_Down(void)
{
  MouseReport report = {0};

  poweroff_count = IDLE_CONNECTION_TIMEOUT/10;
  
  // 1. 左键按下
  report.buttons = BUTTON_LEFT_PRESSED;
  report.x = 0;
  report.y = 0;
  Send_Mouse_Report(&report);
	
  // 2. 连续向下滑动（Y=60，适配抖音阈值）
   report.y = 90;
	Send_Mouse_Report(&report);
  
  // 3. 左键释放
  report.buttons = 0x00;
  report.x = 0;
  report.y = 0;
  Send_Mouse_Report(&report);

  // 4. 核心新增：光标归位（发送反向位移，无按键）
  report.y = -30; // 向上移回原位
  Send_Mouse_Report(&report);
}

void Ebook_Slide_Left(void)
{
  MouseReport report = {0};
  
  poweroff_count = IDLE_CONNECTION_TIMEOUT/10;
  
  // 1. 左键按下
  report.buttons = BUTTON_LEFT_PRESSED;
  report.x = 0;
  report.y = 0;
  Send_Mouse_Report(&report);
	
  // 2. 连续向左滑动
  report.x = -90;
  Send_Mouse_Report(&report);

  // 3. 左键释放
  report.buttons = 0;
  report.x = 0;
  report.y = 0;
	Send_Mouse_Report(&report);

  // 4. 核心新增：光标归位（发送反向位移，无按键）
  report.x = 30; // 向下移回原位
  Send_Mouse_Report(&report);
}

void Ebook_Slide_Right(void)
{
  MouseReport report = {0};

  poweroff_count = IDLE_CONNECTION_TIMEOUT/10;
  
  // 1. 左键按下
  report.buttons = BUTTON_LEFT_PRESSED;
  report.x = 0;
  report.y = 0;
  Send_Mouse_Report(&report);
	
  // 2. 连续向右滑动
   report.x = 90;
	Send_Mouse_Report(&report);
  
  // 3. 左键释放
  report.buttons = 0x00;
  report.x = 0;
  report.y = 0;
  Send_Mouse_Report(&report);

  // 4. 核心新增：光标归位（发送反向位移，无按键）
  report.x = -30; // 向上移回原位
  Send_Mouse_Report(&report);
}

void TouchPoint_Move_Down(void)
{	
  MouseReport report = {0};

  poweroff_count = IDLE_CONNECTION_TIMEOUT/10;
  
	report.buttons = 0x00;
	report.x = 0;
	report.y = 1200;
	Send_Mouse_Report(&report);
}

void TouchPoint_Move_Up(void)
{	
  MouseReport report = {0};

  poweroff_count = IDLE_CONNECTION_TIMEOUT/10;
  
	report.buttons = 0x00;
	report.x = 0;
	report.y = -1200;
	Send_Mouse_Report(&report);
}

void Douyin_Click_likes(void)
{
  MouseReport report = {0};

  poweroff_count = IDLE_CONNECTION_TIMEOUT/10;
  
	report.buttons = 0x00;
	report.x = 0;
	report.y = 0;
	Send_Mouse_Report(&report);
	Delay_10MS(1);
	Send_Mouse_Report(&report);
}

void APP_Tick(void)
{
  /* HID library Tick */
  hidDevice_Tick();  
}

/**** HID/HOGP Callback functions ****/
void hidSetReport_CB(uint8_t ID, uint8_t type, uint8_t len, uint8_t *data)
{
  uint8_t i;

  PRINTF("Set Report ID = 0x%x, Type = 0x%x, len = 0x%x data:", ID, type, len);
  for (i=0; i<len; i++){
    PRINTF("%x - ", data[i]);
  }
  PRINTF("\n");
}

void hidGetReport_CB(uint8_t ID, uint8_t type)
{
  uint8_t ret, len;

  PRINTF("Get Report Callback ID = %d\n", ID);
  if (type == INPUT_REPORT)  
  {  
    uint8_t data[5]={0,0,0,0,0};
    len = 4;
    ret = hidUpdateReportValue(ID, type, len, data);
  }
  else if (type == BOOT_MOUSE_INPUT_REPORT)
  {  
    uint8_t data_BootMouse[3]={0,0,0}; 
    len = 2;
    ret = hidUpdateReportValue(ID, type, len, data_BootMouse);
  }  
  if (ret != BLE_STATUS_SUCCESS)
    PRINTF("Update report value Error during a hidGetReport_CB() procedure: 0x%02x\n", ret);
}

void hidChangeProtocolMode_CB(uint8_t mode)
{
  PRINTF("Protocol mode changed to %x\n", mode);
}

void hidControlPoint_CB(uint8_t value)
{
  if (value == 1)
#ifdef PTS_AUTOMATING 
    PRINTF("(%c%c1) HID Control Point Command: 1 (Exit Suspend)\n", PTS_CODE, HIDS_PTS_CODE);
#else
    PRINTF("Control Point value changed to 1. Host Exit Suspend state\n");
#endif
  else if (value == 0)
#ifdef PTS_AUTOMATING 
    PRINTF("(%c%c2) HID Control Point Command: 0 (Suspend)\n", PTS_CODE, HIDS_PTS_CODE);
#else
    PRINTF("Control Point value changed to 0. Host in Suspend state\n");
#endif   
}

/***************** BlueNRG-LP stack events **********************************/

void hci_disconnection_complete_event(uint8_t Status,
                                      uint16_t Connection_Handle,
                                      uint8_t Reason)
{
  HID_Lib_hci_disconnection_complete_event(Status, Connection_Handle, Reason);
  PRINTF("Disconnection Complete Event\n"); 
}

void hci_le_connection_complete_event(uint8_t Status,
                                      uint16_t Connection_Handle,
                                      uint8_t Role,
                                      uint8_t Peer_Address_Type,
                                      uint8_t Peer_Address[6],
                                      uint16_t Conn_Interval,
                                      uint16_t Conn_Latency,
                                      uint16_t Supervision_Timeout,
                                      uint8_t Master_Clock_Accuracy)
{
  HID_Lib_hci_le_connection_complete_event(Status, Connection_Handle,
                                           Role, Peer_Address_Type,
                                           Peer_Address, Conn_Interval,
                                           Conn_Latency, Supervision_Timeout,
                                           Master_Clock_Accuracy);
  if(Status == 0)
    PRINTF("Connected.\n");      
}

/*******************************************************************************
 * Function Name  : hci_le_enhanced_connection_complete_event.
 * Description    : This event indicates that a new connection has been created
 * Input          : See file bluenrg_lp_events.h
 * Output         : See file bluenrg_lp_events.h
 * Return         : See file bluenrg_lp_events.h
 *******************************************************************************/
void hci_le_enhanced_connection_complete_event(uint8_t Status,
                                               uint16_t Connection_Handle,
                                               uint8_t Role,
                                               uint8_t Peer_Address_Type,
                                               uint8_t Peer_Address[6],
                                               uint8_t Local_Resolvable_Private_Address[6],
                                               uint8_t Peer_Resolvable_Private_Address[6],
                                               uint16_t Conn_Interval,
                                               uint16_t Conn_Latency,
                                               uint16_t Supervision_Timeout,
                                               uint8_t Master_Clock_Accuracy)
{
  
  hci_le_connection_complete_event(Status,
                                   Connection_Handle,
                                   Role,
                                   Peer_Address_Type,
                                   Peer_Address,
                                   Conn_Interval,
                                   Conn_Latency,
                                   Supervision_Timeout,
                                   Master_Clock_Accuracy);
}

void aci_gap_limited_discoverable_event()
{
  HID_Lib_aci_gap_limited_discoverable_event();
}

void aci_gap_pass_key_req_event(uint16_t Connection_Handle)
{
  HID_Lib_aci_gap_pass_key_req_event(Connection_Handle);
}

void aci_gap_pairing_complete_event(uint16_t Connection_Handle,
                                    uint8_t Status,
                                    uint8_t Reason)
{
  HID_Lib_aci_gap_pairing_complete_event(Connection_Handle, Status);
}

void aci_gap_bond_lost_event()
{
  HID_Lib_aci_gap_bond_lost_event();
}

void aci_gatt_srv_attribute_modified_event(uint16_t Connection_Handle,
                                           uint16_t Attr_Handle,
                                           uint16_t Attr_Data_Length,
                                           uint8_t Attr_Data[])
{
  HID_Lib_aci_gatt_srv_attribute_modified_event(Connection_Handle, 
                                                Attr_Handle,
                                                Attr_Data_Length,
                                                Attr_Data);
#ifdef PTS_AUTOMATING   
  if ((Attr_Data_Length == 2) && (Attr_Data[1] == 0x00 && Attr_Data[0] == 0x01))
  {
    PRINTF("(%c%c3) Enabled Char to Notify\n\n", PTS_CODE, HIDS_PTS_CODE);
  }  
#endif  
}

void aci_gatt_read_permit_req_event(uint16_t Connection_Handle,
                                    uint16_t Attribute_Handle,
                                    uint16_t Offset)
{
  HID_Lib_aci_gatt_read_permit_req_event(Connection_Handle, Attribute_Handle,
                                         Offset);
}

void aci_l2cap_connection_update_resp_event(uint16_t Connection_Handle,
                                            uint16_t Result)
{
  HID_Lib_aci_l2cap_connection_update_resp_event(Connection_Handle, Result);
}

void aci_l2cap_proc_timeout_event(uint16_t Connection_Handle,
                                  uint8_t Data_Length,
                                  uint8_t Data[])
{
  HID_Lib_aci_l2cap_proc_timeout_event(Connection_Handle, Data_Length,
                                       Data);
}

