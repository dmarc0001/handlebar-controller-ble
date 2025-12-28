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

BleCombo::BleCombo( std::string deviceName, std::string deviceManufacturer, uint8_t batteryLevel ) : _buttons( 0 ), hid( 0 )
{
  connectionStatus = std::make_shared<BleConnectionStatus>();
  this->deviceName = deviceName;
  this->deviceManufacturer = deviceManufacturer;
  this->batteryLevel = batteryLevel;
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
  // pServer->setCallbacks( static_cast<NimBLEServerCallbacks*>(connectionStatus.get()) );
  pServer->setCallbacks( this );

  Logger.debug( prefs::MYLOG, "BleCombo::begin create HID Info..." );
  hid = new NimBLEHIDDevice( pServer );
  connectionStatus->inputMouse = hid->getInputReport( 0 );  // <-- input REPORTID from report map

  hid->setManufacturer( deviceManufacturer );
  hid->setPnp( 0x02, 0xe502, 0xa111, 0x0210 );
  hid->setHidInfo( 0x00, 0x02 );
  hid->setReportMap( _hidReportDescriptorPtr, hidReportDescriptorSize );
  hid->startServices();

  startAdvertizing();

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
  pAdvertising->setAppearance( HID_MOUSE );
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
  // startAdvertizing();
  Logger.debug( prefs::MYLOG, "BleCombo::onConnect..." );
}

void BleCombo::onDisconnect( NimBLEServer *pServer, NimBLEConnInfo &connInfo, int reason )
{
  connectionStatus->connected = false;
  if ( disconnectCallback )
    disconnectCallback();
  Logger.debug( prefs::MYLOG, "BleCombo::onDisconnect..." );
}

void BleCombo::onConnect( Callback cb )
{
  Logger.debug(prefs::MYLOG, "BleCombo::onConnect set callback");
  connectCallback = cb;
}

void BleCombo::onDisconnect( Callback cb )
{
  Logger.debug(prefs::MYLOG, "BleCombo::onDisconnect set callback");
  disconnectCallback = cb;
}