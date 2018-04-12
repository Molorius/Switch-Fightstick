/*
Nintendo Switch Fightstick - Proof-of-Concept

Based on the LUFA library's Low-Level Joystick Demo
	(C) Dean Camera
Based on the HORI's Pokken Tournament Pro Pad design
	(C) HORI

This project implements a modified version of HORI's Pokken Tournament Pro Pad
USB descriptors to allow for the creation of custom controllers for the
Nintendo Switch. This also works to a limited degree on the PS3.

Since System Update v3.0.0, the Nintendo Switch recognizes the Pokken
Tournament Pro Pad as a Pro Controller. Physical design limitations prevent
the Pokken Controller from functioning at the same level as the Pro
Controller. However, by default most of the descriptors are there, with the
exception of Home and Capture. Descriptor modification allows us to unlock
these buttons for our use.
*/

#include "Joystick.h"
#include "I2CSlave.h"

// 0 buttonmsb, 1 buttonlsb, 2 hat, 3 lx, 4 ly, 5 rx, 6 ry
// msb has MINUX, PLUS, LCLICK, RCLICK, HOME, CAPTURE
// lsb has Y, B, A, X, L, R, ZL, ZR
volatile unsigned char i2c_register_map[I2C_REG_MAX];
volatile uint8_t i2c_register = 0; // current register, for reading

// save the incoming value to the register map
void I2C_received(char* buf,uint8_t len) {
	static uint8_t pos; // position in register
	static uint8_t fin; // final position in buffer
	
	if(len>0) {
		i2c_register = buf[0]; // save the last initial register (for reads)
	}
	if(len>1) {
		pos = buf[0]; // get the map position
		fin = pos+len-1; // final map position
		if(fin > I2C_REG_MAX) len = I2C_REG_MAX-pos; // don't let it go higher than register map
		for(int i=1;i<len;i++) {
			i2c_register_map[pos] = buf[i]; // copy the buffer over
			pos++;
		}
	}
}

// spit out the register map
void I2C_requested() {
	if(i2c_register < I2C_REG_MAX) {
		I2C_transmitByte(i2c_register_map[i2c_register]);
		i2c_register++;
	}
	else {
		I2C_transmitByte(0);
	}
}

void I2C_setup() {
	// initialize register map
	i2c_register_map[0] = 0;
	i2c_register_map[1] = 0;
	i2c_register_map[2] = HAT_CENTER;
	i2c_register_map[3] = STICK_CENTER;
	i2c_register_map[4] = STICK_CENTER;
	i2c_register_map[5] = STICK_CENTER;
	i2c_register_map[6] = STICK_CENTER;

	I2C_setCallbacks(I2C_received, I2C_requested); // setup the callback functions
	I2C_init(I2C_ADDRESS); // set it up!
}

// Main entry point.
int main(void) {
	// We'll start by performing hardware and peripheral setup.
	SetupHardware();
	I2C_setup(); // sets up i2c and the necessary callbacks
	// We'll then enable global interrupts for our use.
	GlobalInterruptEnable();
	// Once that's done, we'll enter an infinite loop.
	for (;;)
	{
		// We need to run our task to process and deliver data for our IN and OUT endpoints.
		HID_Task();
		// We also need to run the main USB management task.
		USB_USBTask();
	}
}

// Configures hardware and peripherals, such as the USB peripherals.
void SetupHardware(void) {
	// We need to disable watchdog if enabled by bootloader/fuses.
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	// We need to disable clock division before initializing the USB hardware.
	clock_prescale_set(clock_div_1);

	// We can then initialize our hardware and peripherals, including the USB stack.
	// The USB stack should be initialized last.
	USB_Init();
}


// Fired to indicate that the device is enumerating.
void EVENT_USB_Device_Connect(void) {
	// We can indicate that we're enumerating here (via status LEDs, sound, etc.).
}

// Fired to indicate that the device is no longer connected to a host.
void EVENT_USB_Device_Disconnect(void) {
	// We can indicate that our device is not ready (via status LEDs, sound, etc.).
}

// Fired when the host set the current configuration of the USB device after enumeration.
void EVENT_USB_Device_ConfigurationChanged(void) {
	bool ConfigSuccess = true;

	// We setup the HID report endpoints.
	ConfigSuccess &= Endpoint_ConfigureEndpoint(JOYSTICK_OUT_EPADDR, EP_TYPE_INTERRUPT, JOYSTICK_EPSIZE, 1);
	ConfigSuccess &= Endpoint_ConfigureEndpoint(JOYSTICK_IN_EPADDR, EP_TYPE_INTERRUPT, JOYSTICK_EPSIZE, 1);

	// We can read ConfigSuccess to indicate a success or failure at this point.
}

// Process control requests sent to the device from the USB host.
void EVENT_USB_Device_ControlRequest(void) {
	// We can handle two control requests: a GetReport and a SetReport.

	// Not used here, it looks like we don't receive control request from the Switch.
}


// Process and deliver data from IN and OUT endpoints.
void HID_Task(void) {
	// If the device isn't connected and properly configured, we can't do anything here.
	if (USB_DeviceState != DEVICE_STATE_Configured)
		return;

	// We'll start with the OUT endpoint.
	Endpoint_SelectEndpoint(JOYSTICK_OUT_EPADDR);
	// We'll check to see if we received something on the OUT endpoint.
	if (Endpoint_IsOUTReceived())
	{
		// If we did, and the packet has data, we'll react to it.
		if (Endpoint_IsReadWriteAllowed())
		{
			// We'll create a place to store our data received from the host.
			USB_JoystickReport_Output_t JoystickOutputData;
			// We'll then take in that data, setting it up in our storage.
			while(Endpoint_Read_Stream_LE(&JoystickOutputData, sizeof(JoystickOutputData), NULL) != ENDPOINT_RWSTREAM_NoError);
			// At this point, we can react to this data.

			// However, since we're not doing anything with this data, we abandon it.
		}
		// Regardless of whether we reacted to the data, we acknowledge an OUT packet on this endpoint.
		Endpoint_ClearOUT();
	}

	// We'll then move on to the IN endpoint.
	Endpoint_SelectEndpoint(JOYSTICK_IN_EPADDR);
	// We first check to see if the host is ready to accept data.
	if (Endpoint_IsINReady())
	{
		// We'll create an empty report.
		USB_JoystickReport_Input_t JoystickInputData;
		// We'll then populate this report with what we want to send to the host.
		GetNextReport(&JoystickInputData);
		// Once populated, we can output this data to the host. We do this by first writing the data to the control stream.
		while(Endpoint_Write_Stream_LE(&JoystickInputData, sizeof(JoystickInputData), NULL) != ENDPOINT_RWSTREAM_NoError);
		// We then send an IN packet on this endpoint.
		Endpoint_ClearIN();
	}
}

// Prepare the next report for the host.
void GetNextReport(USB_JoystickReport_Input_t* const ReportData) {

	// Prepare an empty report
	memset(ReportData, 0, sizeof(USB_JoystickReport_Input_t));

	// Stop the interrupts (USB, I2C) so we can safely get controller map
	GlobalInterruptDisable();

	// save the data
	ReportData->Button = ((uint16_t)i2c_register_map[0] << 8) | i2c_register_map[1];
	ReportData->HAT = i2c_register_map[2];
	ReportData->LX = i2c_register_map[3];
	ReportData->LY = i2c_register_map[4];
	ReportData->RX = i2c_register_map[5];
	ReportData->RY = i2c_register_map[6];

	// re-enable the interrupts
	GlobalInterruptEnable();
}
