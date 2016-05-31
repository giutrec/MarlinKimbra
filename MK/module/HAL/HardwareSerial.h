/**
 * MK & MK4due 3D Printer Firmware
 *
 * Based on Marlin, Sprinter and grbl
 * Copyright (C) 2011 Camiel Gubbels / Erik van der Zalm
 * Copyright (C) 2013 - 2016 Alberto Cotronei @MagoKimbra
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */

/**
 * HardwareSerial.h - Hardware serial library for Wiring
 * Copyright (c) 2006 Nicholas Zambetti.  All right reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Modified 28 September 2010 by Mark Sproul
 * Modified  3 March 2015 by MagoKimbra
 */

#ifndef HardwareSerial_H
  #define HardwareSerial_H

  #define clear_bit(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
  #define set_bit(sfr, bit)   (_SFR_BYTE(sfr) |= _BV(bit))

  // The presence of the UBRRH register is used to detect a UART.
  #define UART_PRESENT(port) ((port == 0 && (defined(UBRRH) || defined(UBRR0H))) || \
              (port == 1 && defined(UBRR1H)) || (port == 2 && defined(UBRR2H)) || \
              (port == 3 && defined(UBRR3H)))

  // These are macros to build serial port register names for the selected SERIAL_PORT (C preprocessor
  // requires two levels of indirection to expand macro values properly)
  #define SERIAL_REGNAME(registerbase,number,suffix) SERIAL_REGNAME_INTERNAL(registerbase,number,suffix)
  #if SERIAL_PORT == 0 && (!defined(UBRR0H) || !defined(UDR0)) // use un-numbered registers if necessary
    #define SERIAL_REGNAME_INTERNAL(registerbase,number,suffix) registerbase##suffix
  #else
    #define SERIAL_REGNAME_INTERNAL(registerbase,number,suffix) registerbase##number##suffix
  #endif

  // Registers used by MarlinSerial class (these are expanded 
  // depending on selected serial port
  #define M_UCSRxA SERIAL_REGNAME(UCSR,SERIAL_PORT,A) // defines M_UCSRxA to be UCSRnA where n is the serial port number
  #define M_UCSRxB SERIAL_REGNAME(UCSR,SERIAL_PORT,B) 
  #define M_RXENx SERIAL_REGNAME(RXEN,SERIAL_PORT,)    
  #define M_TXENx SERIAL_REGNAME(TXEN,SERIAL_PORT,)    
  #define M_RXCIEx SERIAL_REGNAME(RXCIE,SERIAL_PORT,)    
  #define M_UDREx SERIAL_REGNAME(UDRE,SERIAL_PORT,)    
  #define M_UDRx SERIAL_REGNAME(UDR,SERIAL_PORT,)  
  #define M_UBRRxH SERIAL_REGNAME(UBRR,SERIAL_PORT,H)
  #define M_UBRRxL SERIAL_REGNAME(UBRR,SERIAL_PORT,L)
  #define M_RXCx SERIAL_REGNAME(RXC,SERIAL_PORT,)
  #define M_USARTx_RX_vect SERIAL_REGNAME(USART,SERIAL_PORT,_RX_vect)
  #define M_U2Xx SERIAL_REGNAME(U2X,SERIAL_PORT,)

  #define DEC 10
  #define HEX 16
  #define OCT 8
  #define BIN 2
  #define BYTE 0

  #define RX_BUFFER_SIZE 128

  struct ring_buffer {
    unsigned char buffer[RX_BUFFER_SIZE];
    int head;
    int tail;
  };

  #if UART_PRESENT(SERIAL_PORT)
    extern ring_buffer rx_buffer;
  #endif

  class MKHardwareSerial {
    public:
      MKHardwareSerial();
      void begin(long);
      void end();
      int peek(void);
      int read(void);
      void flush(void);

      FORCE_INLINE int available(void) {
        return (unsigned int)(RX_BUFFER_SIZE + rx_buffer.head - rx_buffer.tail) % RX_BUFFER_SIZE;
      }

      FORCE_INLINE void write(uint8_t c) {
        while (!TEST(M_UCSRxA, M_UDREx));
        M_UDRx = c;
      }

      FORCE_INLINE void checkRx(void) {
        if (TEST(M_UCSRxA, M_RXCx)) {
          unsigned char c  =  M_UDRx;
          int i = (unsigned int)(rx_buffer.head + 1) % RX_BUFFER_SIZE;
          if (i != rx_buffer.tail) {
            rx_buffer.buffer[rx_buffer.head] = c;
            rx_buffer.head = i;
          }
        }
      }

    private:
      void printNumber(unsigned long, uint8_t);
      void printFloat(double, uint8_t);

    public:
      FORCE_INLINE void write(const char *str) { while (*str) write(*str++); }
      FORCE_INLINE void write(const uint8_t *buffer, size_t size) { while (size--) write(*buffer++); }
      FORCE_INLINE void print(const String &s) { for (int i = 0; i < (int)s.length(); i++) write(s[i]); }
      FORCE_INLINE void print(const char *str) { write(str); }

      void print(char, int = BYTE);
      void print(unsigned char, int = BYTE);
      void print(int, int = DEC);
      void print(unsigned int, int = DEC);
      void print(long, int = DEC);
      void print(unsigned long, int = DEC);
      void print(double, int = 2);

      void println(const String &s);
      void println(const char[]);
      void println(char, int = BYTE);
      void println(unsigned char, int = BYTE);
      void println(int, int = DEC);
      void println(unsigned int, int = DEC);
      void println(long, int = DEC);
      void println(unsigned long, int = DEC);
      void println(double, int = 2);
      void println(void);
  };

  extern MKHardwareSerial MKSerial;

#endif // HardwareSerial_H
