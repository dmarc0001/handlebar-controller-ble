#include "BleCombo.hpp"

#include <NimBLEDevice.h>
#include "HIDTypes.h"

#include "sdkconfig.h"

#if defined( CONFIG_ARDUHAL_ESP_LOG )
#include "esp32-hal-log.h"
#define LOG_TAG ""
#else
#include "esp_log.h"
static const char *LOG_TAG = "NimBLEDevice";
#endif

#include <Elog.h>
#include "appPreferences.hpp"
#include "BleHidDescriptors.hpp"
#include "BleHidAsciiMap.hpp"

BleCombo::BleCombo( std::string _deviceName, std::string _deviceManufacturer, uint8_t _batteryLevel )
    : _buttons( 0 ), hid( nullptr ), deviceName( _deviceName ), deviceManufacturer( _deviceManufacturer ), batteryLevel( _batteryLevel )
{
  connectionStatus = std::make_shared< BleConnectionStatus >();
  memset( static_cast< void * >( &_keyReport ), 0, sizeof( KeyReport ) );
}

void BleCombo::begin( void )
{
  Logger.debug( prefs::MYLOG, "BleCombo::begin..." );
  NimBLEDevice::init( deviceName );
  // BLEDevice::setSecurityAuth(BLE_SM_PAIR_AUTHREQ_BOND);
  BLEDevice::setSecurityAuth( true, true, false );

  Logger.debug( prefs::MYLOG, "BleCombo::begin create Server..." );
  NimBLEServer *pServer = NimBLEDevice::createServer();
  Logger.debug( prefs::MYLOG, "BleCombo::begin set callbacks..." );
  pServer->setCallbacks( this );
  //
  // HID Devices
  //
  Logger.debug( prefs::MYLOG, "BleCombo::begin create HID Info..." );
  // mouse device
  hid = std::make_shared< NimBLEHIDDevice >( pServer );
  if ( hid.get() == nullptr )
  {
    Logger.error( prefs::MYLOG, "BleCombo::begin...hid is nullptr!" );
    return;
  }
  // mouse device
  connectionStatus->inputMouse = hid->getInputReport( MOUSE_ID );  // <-- input REPORTID from report map
  if ( connectionStatus->inputMouse == nullptr )
  {
    Logger.error( prefs::MYLOG, "BleCombo::begin...inputMouse is nullptr!" );
    return;
  }
  else
  {
    Logger.debug( prefs::MYLOG, "BleCombo::begin...input mouse created" );
  }
  // keyboard device
  connectionStatus->inputKeyboard = hid->getInputReport( KEYBOARD_ID );  // <-- input REPORTID from report map
  if ( connectionStatus->inputKeyboard == nullptr )
  {
    Logger.error( prefs::MYLOG, "BleCombo::begin...inputKeyboard is nullptr!" );
    return;
  }
  else
  {
    Logger.debug( prefs::MYLOG, "BleCombo::begin...input keyboard created" );
  }
  connectionStatus->outputKeyboard = hid->getOutputReport( KEYBOARD_ID );
  if ( connectionStatus->outputKeyboard == nullptr )
  {
    Logger.error( prefs::MYLOG, "BleCombo::begin...outputKeyboard is nullptr!" );
    return;
  }
  else
  {
    Logger.debug( prefs::MYLOG, "BleCombo::begin...output keyboard created" );
  }
  connectionStatus->outputKeyboard->setCallbacks( this );
  // complete hid setup
  hid->setManufacturer( deviceManufacturer );
  hid->setPnp( 0x02, 0xe502, 0xa111, 0x0210 );
  hid->setHidInfo( 0x00, 0x02 );
  hid->setReportMap( _hidReportDescriptorPtr, hidReportDescriptorSize );
  hid->startServices();
  // make the device visible
  startAdvertizing();
  // etc...
  hid->setBatteryLevel( batteryLevel );
  Logger.debug( prefs::MYLOG, "BleCombo::begin...OK" );
}

void BleCombo::end( void )
{
  NimBLEDevice::deinit( true );
}

void BleCombo::startAdvertizing()
{
  Logger.debug( prefs::MYLOG, "BleCombo::begin start advertizing..." );
  NimBLEServer *pServer = NimBLEDevice::getServer();
  NimBLEAdvertising *pAdvertising = pServer->getAdvertising();
  // pAdvertising->setAppearance( HID_MOUSE );
  pAdvertising->setAppearance( GENERIC_HID );
  pAdvertising->addServiceUUID( hid->getHidService()->getUUID() );
  pAdvertising->setName( this->deviceName );
  pAdvertising->setManufacturerData( this->deviceManufacturer );
  pAdvertising->start();
}

bool BleCombo::isAdvertizing()
{
  NimBLEServer *pServer = NimBLEDevice::getServer();
  NimBLEAdvertising *pAdvertising = pServer->getAdvertising();
  return pAdvertising->isAdvertising();
}

void BleCombo::rawAction( uint8_t msg[], char msgSize )
{
  // i dont know what this mADE
  if ( this->isConnected() )
  {
    connectionStatus->inputMouse->setValue( msg, msgSize );
    connectionStatus->inputMouse->notify();
  }
}

void BleCombo::m_click( uint8_t b )
{
  _buttons = b;
  m_move( 0, 0, 0, 0 );
  _buttons = 0;
  m_move( 0, 0, 0, 0 );
}

void BleCombo::m_move( signed char x, signed char y, signed char wheel, signed char hWheel )
{
  if ( this->isConnected() )
  {
    uint8_t m[ 5 ];
    m[ 0 ] = _buttons;
    m[ 1 ] = x;
    m[ 2 ] = y;
    m[ 3 ] = wheel;
    m[ 4 ] = hWheel;
    connectionStatus->inputMouse->setValue( m, 5 );
    connectionStatus->inputMouse->notify();
  }
}

void BleCombo::m_buttons( uint8_t b )
{
  if ( b != _buttons )
  {
    _buttons = b;
    m_move( 0, 0, 0, 0 );
  }
}

void BleCombo::m_press( uint8_t b )
{
  m_buttons( _buttons | b );
}

void BleCombo::m_release( uint8_t b )
{
  m_buttons( _buttons & ~b );
}

bool BleCombo::m_isPressed( uint8_t b ) const
{
  if ( ( b & _buttons ) > 0 )
    return true;
  return false;
}

// press() adds the specified key (printing, non-printing, or modifier)
// to the persistent key report and sends the report.  Because of the way
// USB HID works, the host acts like the key remains pressed until we
// call release(), releaseAll(), or otherwise clear the report and resend.
size_t BleCombo::k_press( uint8_t k )
{
  uint8_t i;
  if ( k >= 136 )
  {  // it's a non-printing key (not a modifier)
    k = k - 136;
  }
  else if ( k >= 128 )
  {  // it's a modifier key
    _keyReport.modifiers |= ( 1 << ( k - 128 ) );
    k = 0;
  }
  else
  {  // it's a printing key
    k = pgm_read_byte( _asciimap + k );
    if ( !k )
    {
      setWriteError();
      return 0;
    }
    if ( k & 0x80 )
    {                                // it's a capital letter or other character reached with shift
      _keyReport.modifiers |= 0x02;  // the left shift modifier
      k &= 0x7F;
    }
  }

  // Add k to the key report only if it's not already present
  // and if there is an empty slot.
  if ( _keyReport.keys[ 0 ] != k && _keyReport.keys[ 1 ] != k && _keyReport.keys[ 2 ] != k && _keyReport.keys[ 3 ] != k &&
       _keyReport.keys[ 4 ] != k && _keyReport.keys[ 5 ] != k )
  {
    for ( i = 0; i < 6; i++ )
    {
      if ( _keyReport.keys[ i ] == 0x00 )
      {
        _keyReport.keys[ i ] = k;
        break;
      }
    }
    if ( i == 6 )
    {
      setWriteError();
      return 0;
    }
  }
  k_sendReport( &_keyReport );
  return 1;
}

// release() takes the specified key out of the persistent key report and
// sends the report.  This tells the OS the key is no longer pressed and that
// it shouldn't be repeated any more.
size_t BleCombo::k_release( uint8_t k )
{
  uint8_t i;
  if ( k >= 136 )
  {  // it's a non-printing key (not a modifier)
    k = k - 136;
  }
  else if ( k >= 128 )
  {  // it's a modifier key
    _keyReport.modifiers &= ~( 1 << ( k - 128 ) );
    k = 0;
  }
  else
  {  // it's a printing key
    k = pgm_read_byte( _asciimap + k );
    if ( !k )
    {
      return 0;
    }
    if ( k & 0x80 )
    {                                     // it's a capital letter or other character reached with shift
      _keyReport.modifiers &= ~( 0x02 );  // the left shift modifier
      k &= 0x7F;
    }
  }

  // Test the key report to see if k is present.  Clear it if it exists.
  // Check all positions in case the key is present more than once (which it shouldn't be)
  for ( i = 0; i < 6; i++ )
  {
    if ( 0 != k && _keyReport.keys[ i ] == k )
    {
      _keyReport.keys[ i ] = 0x00;
    }
  }

  k_sendReport( &_keyReport );
  return 1;
}

void BleCombo::k_releaseAll( void )
{
  _keyReport.keys[ 0 ] = 0;
  _keyReport.keys[ 1 ] = 0;
  _keyReport.keys[ 2 ] = 0;
  _keyReport.keys[ 3 ] = 0;
  _keyReport.keys[ 4 ] = 0;
  _keyReport.keys[ 5 ] = 0;
  _keyReport.modifiers = 0;
  k_sendReport( &_keyReport );
}

void BleCombo::k_sendReport( KeyReport *keys )
{
  Logger.debug( prefs::MYLOG, "BleCombo::sendReport " );
  Logger.debug( prefs::MYLOG, " 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x mod 0x%x", keys->keys[ 0 ], keys->keys[ 1 ], keys->keys[ 2 ],
                keys->keys[ 3 ], keys->keys[ 4 ], keys->keys[ 5 ], keys->modifiers );
  if ( isConnected() )
  {
    connectionStatus->inputKeyboard->setValue( ( uint8_t * ) keys, sizeof( KeyReport ) );
    connectionStatus->inputKeyboard->notify();
  }
  Logger.debug( prefs::MYLOG, "BleCombo::sendReport key...OK" );
}

size_t BleCombo::write( uint8_t c )
{
  Logger.debug( prefs::MYLOG, "BleCombo::write char: 0x%x...", c );
  uint8_t p = k_press( c );  // Keydown
  vTaskDelay( 3 );
  k_release( c );  // Keyup
  Logger.debug( prefs::MYLOG, "BleCombo::write char: 0x%x... OK", c );
  return p;  // just return the result of press() since release() almost always returns 1
}

size_t BleCombo::write( const uint8_t *buffer, size_t size )
{
  size_t n = 0;
  while ( size-- )
  {
    if ( *buffer != '\r' )
    {
      if ( write( *buffer ) )
      {
        n++;
      }
      else
      {
        break;
      }
    }
    buffer++;
  }
  return n;
}

bool BleCombo::isConnected( void ) const
{
  return connectionStatus->connected;
}

void BleCombo::setBatteryLevel( uint8_t level )
{
  this->batteryLevel = level;
  if ( hid != 0 )
    this->hid->setBatteryLevel( this->batteryLevel );
}

void BleCombo::onConnect( NimBLEServer *pServer, NimBLEConnInfo &connInfo )
{
  connectionStatus->connected = true;
  if ( connectCallback )
    connectCallback();
  Logger.debug( prefs::MYLOG, "BleCombo::onConnect..." );
}

void BleCombo::onDisconnect( NimBLEServer *pServer, NimBLEConnInfo &connInfo, int reason )
{
  connectionStatus->connected = false;
  if ( disconnectCallback )
    disconnectCallback();
  Logger.debug( prefs::MYLOG, "BleCombo::onDisconnect..." );
}

void BleCombo::onWrite( NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo )
{
  // from NimBLECharacteristicCallbacks
  uint8_t *value = ( uint8_t * ) ( pCharacteristic->getValue().c_str() );
  Logger.debug( prefs::MYLOG, "BleCombo::onWrite special keys: %d...", *value );
}

void BleCombo::onConnect( Callback cb )
{
  Logger.debug( prefs::MYLOG, "BleCombo::onConnect set callback" );
  connectCallback = cb;
}

void BleCombo::onDisconnect( Callback cb )
{
  Logger.debug( prefs::MYLOG, "BleCombo::onDisconnect set callback" );
  disconnectCallback = cb;
}