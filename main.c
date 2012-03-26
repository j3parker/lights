//#define __AVR_ATmega48P__
#include <avr/io.h>
#include <util/delay_basic.h>

/* Port B:
   PB0 (output) front servo control signal
   PB1 (output) back servo control signal
   PB4 (output) diagnostic LED
   */
#define PORTB_CONFIG 0b00011111

/* Port C:
   PC0 (digital input)  force-sensitive resistor
  */
#define PORTC_CONFIG 0b00000000

#define SERVO_FRONT 0b00000001
#define SERVO_BACK  0b00000010
#define LED         0b00010000

#define SERVO_OFF 7 
#define SERVO_PARK 15
#define SERVO_ON   23

static inline void delay_0point1ms(uint16_t time) {
  uint16_t i;
  for ( i = 0; i < time; ++i) {
    _delay_loop_2(25);
  }
}

static inline void do_servo (uint8_t port, uint8_t pulse_width, uint16_t time) {
  uint16_t elapsed = 0;
  while (elapsed < time) {
    PORTB |= port;
    delay_0point1ms(pulse_width);
    PORTB &= ~port;
    delay_0point1ms(200 - pulse_width);
    elapsed += 200;
  }
}

int main(void) {
  DDRC = PORTC_CONFIG; // all C ports input
  DDRB = PORTB_CONFIG;

	uint8_t prev_door = 0xFF;
  while (1) {
    uint8_t door_sensor = !(PINC & 0b00000001);
    if (door_sensor != prev_door) {
      prev_door = door_sensor;
      PORTB ^= LED;
      do_servo(SERVO_FRONT, door_sensor ? SERVO_OFF : SERVO_ON, 6000);
      do_servo(SERVO_BACK, !door_sensor ? SERVO_OFF : SERVO_ON, 6000);
      do_servo(SERVO_FRONT, SERVO_PARK, 3000);
      do_servo(SERVO_BACK, SERVO_PARK, 3000);
      delay_0point1ms(10000);
      PORTB ^= LED;
    }
    delay_0point1ms(1000);
  }
}

