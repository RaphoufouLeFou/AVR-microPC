
#include <util/delay.h>

extern "C" {

/// @brief Output a pixel to the screen at the given address with the given color
/// @param Color 16-bit color value (5 bits red, 6 bits green, 5 bits blue)
/// @param AddressX X coordinate of the pixel (0-127)
/// @param AddressY Y coordinate of the pixel (0-63)
extern void AVR_Output_Pixel(uint16_t Color, uint8_t AddressX, uint8_t AddressY);


/// @brief Fill the screen with the given color
/// @param Color 16-bit color value (5 bits red, 6 bits green, 5 bits blue)
extern void AVR_Output_All(uint16_t Color);


/// @brief Initialize the IO pins 
/// @param None
extern void AVR_Init(void);


/// @brief Read the input pins and return the value
/// @param None
/// @return input value 
extern uint8_t AVR_Read_Inputs(void);


/// @brief Draw a line on the screen between the given coordinates with the given color
/// @param Color 16-bit color value (5 bits red, 6 bits green, 5 bits blue)
/// @param AddressX1 X coordinate of the first point (0-127)
/// @param AddressY1 Y coordinate of the first point (0-63)
/// @param AddressX2 X coordinate of the second point (0-127)
/// @param AddressY2 Y coordinate of the second point (0-63)
extern void AVR_Draw_Line(uint16_t Color, uint8_t AddressX1, uint8_t AddressY1, uint8_t AddressX2, uint8_t AddressY2);


/// @brief Draw a filled rectangle on the screen between the given coordinates with the given color and with the given size
/// @param Color 16-bit color value (5 bits red, 6 bits green, 5 bits blue)
/// @param AddressX1 X coordinate of the first point (0-127)
/// @param AddressY1 Y coordinate of the first point (0-63)
/// @param SizeX X Size (0-127)
/// @param SizeY Y Size (0-63)
extern void AVR_Draw_Fill_Rect(uint16_t Color, uint8_t AddressX1, uint8_t AddressY1, uint8_t SizeX, uint8_t SizeY);


/// @brief Initializes the UART interface with the given prescaler (NOT WORKING YET !!!)
/// @param prescaler found with ( F_CPU / 16 * baudrate) -1 
extern void initUART(uint16_t prescaler);

/// @brief Send a byte over the UART interface (NOT WORKING YET !!!)
/// @param data byte to send
extern void sendUART(uint8_t data);

}

/// @brief Initializes the UART interface with the given baudrate (NOT WORKING YET !!!)
/// @param baudrate 
void AVR_StartSerial(int baudrate){
    initUART((F_CPU/16*baudrate)-1);
}



