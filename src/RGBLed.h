#ifndef RGBLed_h
#define RGBLed_h

#include "Arduino.h"

// Forward declaration for I2C PWM driver (assuming Adafruit PWM Servo Driver Library)
#ifdef USE_I2C_PWM
#include <Adafruit_PWMServoDriver.h>
#endif

// Pattern types
enum PatternType
{
	PATTERN_STATIC,
	PATTERN_FADE,
	PATTERN_BLINK,
	PATTERN_CROSSFADE
};

// Static color pattern structure
struct StaticPattern
{
	uint8_t r, g, b;
	unsigned long duration; // Duration to hold this color (ms)
};

// Fade pattern structure
struct FadePattern
{
	uint8_t r_start, g_start, b_start;
	uint8_t r_end, g_end, b_end;
	unsigned long time_on;	// Fade duration (ms)
	unsigned long time_off; // Off time after fade (ms)
};

// Blink pattern structure
struct BlinkPattern
{
	uint8_t r, g, b;
	unsigned long time_on;	// On duration (ms)
	unsigned long time_off; // Off duration (ms)
	uint8_t repeat_count;	// 0 = infinite
};

// Crossfade pattern structure
struct CrossfadePattern
{
	uint8_t r_from, g_from, b_from;
	uint8_t r_to, g_to, b_to;
	unsigned long fade_duration;
	unsigned long hold_duration;
	bool reverse; // reverse
};

// Generic pattern structure
struct Pattern
{
	PatternType type;
	union
	{
		StaticPattern static_pattern;
		FadePattern fade_pattern;
		BlinkPattern blink_pattern;
		CrossfadePattern crossfade_pattern;
	};
	bool repeat; // Should pattern repeat?
};

// Connection types
enum ConnectionType
{
	DIRECT_CONNECTION,
	I2C_PWM_CONNECTION
};

class RGBLed
{
public:
	// Constructor for direct connection
	RGBLed(int red, int green, int blue, bool common);

// Constructor for I2C PWM connection
#ifdef USE_I2C_PWM
	RGBLed(Adafruit_PWMServoDriver *pwm, uint8_t red_channel, uint8_t green_channel, uint8_t blue_channel, bool common);
#endif

	void off();

	void brightness(int rgb[3], int brightness);
	void brightness(int red, int green, int blue, int brightness);
	void brightness(int brightness);

	void flash(int rgb[3], int duration);
	void flash(int rgb[3], int onDuration, int duration);
	void flash(int red, int green, int blue, int duration);
	void flash(int red, int green, int blue, int onDuration, int duration);

	void setColor(int rgb[3]);
	void setColor(int red, int green, int blue);

	void fadeOut(int rgb[3], int steps, int duration);
	void fadeOut(int red, int green, int blue, int steps, int duration);

	void fadeIn(int rgb[3], int steps, int duration);
	void fadeIn(int red, int green, int blue, int steps, int duration);

	void crossFade(int rgbFrom[3], int rgbTo[3], int steps, int duration);
	void crossFade(int fromRed, int fromGreen, int fromBlue, int toRed, int toGreen, int toBlue, int steps, int duration);

	void gradient(int rgbFrom[3], int rgbTo[3], int step);
	void gradient(int fromRed, int fromGreen, int fromBlue, int toRed, int toGreen, int toBlue, int step);

	// New pattern functions
	void callPattern(Pattern *pattern);
	void updatePattern(unsigned long now); // Call this in main loop
	void stopPattern();
	bool isPatternActive();

	// Helper functions to create patterns
	static Pattern createStaticPattern(uint8_t r, uint8_t g, uint8_t b, unsigned long duration, bool repeat = false);
	static Pattern createFadePattern(uint8_t r_start, uint8_t g_start, uint8_t b_start,
									 uint8_t r_end, uint8_t g_end, uint8_t b_end,
									 unsigned long time_on, unsigned long time_off, bool repeat = false);
	static Pattern createBlinkPattern(uint8_t r, uint8_t g, uint8_t b,
									  unsigned long time_on, unsigned long time_off,
									  uint8_t repeat_count = 0, bool repeat = true);
	static Pattern createCrossfadePattern(uint8_t r_from, uint8_t g_from, uint8_t b_from,
										  uint8_t r_to, uint8_t g_to, uint8_t b_to,
										  unsigned long fade_duration, unsigned long hold_duration,
										  bool reverse = true, bool repeat = true);

	static int RED[3];
	static int GREEN[3];
	static int BLUE[3];
	static int MAGENTA[3];
	static int CYAN[3];
	static int YELLOW[3];
	static int WHITE[3];

	static bool COMMON_ANODE;
	static bool COMMON_CATHODE;

private:
	// Connection parameters
	ConnectionType _connection_type;

	// Direct connection pins
	int _red_pin, _green_pin, _blue_pin;

#ifdef USE_I2C_PWM
	// I2C PWM connection
	Adafruit_PWMServoDriver *_pwm_driver;
	uint8_t _red_channel, _green_channel, _blue_channel;
#endif

	bool _common;
	int _brightness;

	// Pattern state variables
	Pattern *_current_pattern;
	bool _pattern_active;
	unsigned long _pattern_start_time;
	unsigned long _last_update_time;
	uint8_t _pattern_state; // State machine variable
	uint8_t _blink_count;	// For blink patterns
	float _fade_progress;	// For fade patterns (0.0 to 1.0)
	bool _fade_direction;	// For crossfade reverse

	// Private methods
	void color(int red, int green, int blue);
	void blink(int red, int green, int blue, int onDuration, int duration);
	void intensity(int red, int green, int blue, int brightness);
	void fade(int red, int green, int blue, int steps, int duration, bool out);
	void fade(int red, int green, int blue, int steps, int duration, int value, bool wait);

	// Pattern execution methods
	void executeStaticPattern(unsigned long now);
	void executeFadePattern(unsigned long now);
	void executeBlinkPattern(unsigned long now);
	void executeCrossfadePattern(unsigned long now);

	// Hardware abstraction
	void writePWM(uint8_t channel_or_pin, uint16_t value, bool is_channel);
	void initHardware();
};

#endif