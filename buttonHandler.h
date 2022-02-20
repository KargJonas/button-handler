#pragma once
#include <Arduino.h>
#include <ArduinoSTL.h>
#include <map>

/**
 * @brief Contains pin number, state at time of last update,
 *        handlers, as well as a boolean value for storing
 *        information about if handlers have been registered.
 * 
 */
class Button {
public:
  byte pin;
  bool state;
  void (*pressHandler)();
  void (*releaseHandler)();
  bool hasHandlers;

  /**
   * @brief Construct a new Button object without handlers.
   * 
   * @param _pin GPIO pin number.
   */
  Button( byte _pin )
  : pin(_pin),
    state(false),
    hasHandlers(false)
  { }

  /**
   * @brief Construct a new Button object with handlers.
   * 
   * @param _pin GPIO pin number.
   * @param _pressHandler Event handler for the press event.
   * @param _releaseHandler Event handler for the release event.
   */
  Button(
    byte _pin,
    void (*_pressHandler)(),
    void (*_releaseHandler)()
  )
  : pin(_pin),
    pressHandler(_pressHandler),
    releaseHandler(_releaseHandler),
    state(false),
    hasHandlers(true)
  { }
};

/**
 * @brief ButtonHandler class. Enables registering of buttons and handlers.
 * 
 */
class ButtonHandler {
private:
  std::vector<Button> buttons;
  std::map<int, int> pinMap; 
  void (*pressHandler)(int);
  void (*releaseHandler)(int);
  bool hasHandlers;

public:
  /**
   * @brief Construct a new Button Handler object without handlers.
   * 
   */
  ButtonHandler() : hasHandlers(false) { }

  /**
   * @brief Construct a new Button Handler object with handlers.
   * 
   * @param _pressHandler   Event handler for the press event.
   *                        Called when any button is pressed.
   *                        Passes the pin number to the handler function.
   * @param _releaseHandler Event handler for the release event.
   *                        Called when any button is released.
   *                        Passes the pin number to the handler function.
   */
  ButtonHandler( void (*_pressHandler)(int), void (*_releaseHandler)(int) )
  : pressHandler(_pressHandler), releaseHandler(_releaseHandler), hasHandlers(true) { }

  /**
   * @brief Register a new button without handlers.
   * 
   * @param _pin GPIO pin number.
   */
  void registerButton( byte _pin ) {
    Button registeredButton = Button(_pin);
    buttons.push_back(registeredButton);
    pinMap[_pin] = buttons.size() - 1;
  }

  /**
   * @brief Register multiple buttons without handlers.
   * 
   * @param _pins An std::vector<byte> list containing pin numbers.
   */
  void registerButton( std::vector<byte> _pins ) {
    for ( byte _pin : _pins ) {
      registerButton(_pin);
      pinMap[_pin] = buttons.size() - 1;
    }
  }

  /**
   * @brief Register a new button with handlers.
   * 
   * @param _pin GPIO pin number.
   * @param _pressHandler Event handler for the press event.
   * @param _releaseHandler Event handler for the release event.
   */
  void registerButton(
    byte _pin,
    void (*_pressHandler)(),
    void (*_releaseHandler)() )
  {
    Button registeredButton = Button(_pin, _pressHandler, _releaseHandler);
    buttons.push_back(registeredButton);
    pinMap[_pin] = buttons.size() - 1;
  }

  /**
   * @brief Set press and release handlers.
   *        NOTE: This function is called AFTER the handlers
   *        attached by ButtonHandler.registerButton()
   * 
   * @param _pressHandler   Event handler for the press event.
   *                        Called when any button is pressed.
   *                        Passes the pin number to the handler function.
   * @param _releaseHandler Event handler for the release event.
   *                        Called when any button is released.
   *                        Passes the pin number to the handler function.
   */
  void registerHandlers( void (*_pressHandler)(int), void (*_releaseHandler)(int) ) {
    pressHandler = _pressHandler;
    releaseHandler = _releaseHandler;
    hasHandlers = true;
  }

  /**
   * @brief Get the state of a button by its pin number.
   * 
   * @param _pin GPIO pin number.
   * @return true Button is pressed.
   * @return false Button is not pressed.
   */
  bool getState(int _pin) {
    return buttons[pinMap[_pin]].state;
  }

  /**
   * @brief Checks if any of the button states has changed and
   *        calls the appropriate handlers any change has occurred.
   * 
   */
  void update() {
    for ( Button &button : buttons ) {
      bool newState = (digitalRead(button.pin) == HIGH);

      if ( newState != button.state ) {
        button.state = newState;

        if ( newState ) {
          if ( button.hasHandlers ) button.pressHandler();
          if (hasHandlers) pressHandler(button.pin);
        } else {
          if ( button.hasHandlers ) button.releaseHandler();
          if (hasHandlers) releaseHandler(button.pin);
        }
      }
    }
  }
};
