#include "Arduino.h"
#include "RGBLed.h"

// Static color definitions
int RGBLed::RED[3] = {255, 0, 0};
int RGBLed::GREEN[3] = {0, 255, 0};
int RGBLed::BLUE[3] = {0, 0, 255};
int RGBLed::MAGENTA[3] = {255, 0, 255};
int RGBLed::CYAN[3] = {0, 255, 255};
int RGBLed::YELLOW[3] = {255, 255, 0};
int RGBLed::WHITE[3] = {255, 255, 255};

bool RGBLed::COMMON_ANODE = true;
bool RGBLed::COMMON_CATHODE = false;

// Constructor for direct connection
RGBLed::RGBLed(int red, int green, int blue, bool common) 
    : _connection_type(DIRECT_CONNECTION),
      _red_pin(red), 
      _green_pin(green), 
      _blue_pin(blue), 
      _common(common), 
      _brightness(100),
      _current_pattern(nullptr),
      _pattern_active(false),
      _pattern_start_time(0),
      _last_update_time(0),
      _pattern_state(0),
      _blink_count(0),
      _fade_progress(0.0f),
      _fade_direction(true)
{
#if defined(ESP32)
    // ESP32-S2 compatible LEDC setup
    ledcSetup(0, 5000, 8);      // Channel 0, 5kHz, 8-bit resolution
    ledcSetup(1, 5000, 8);      // Channel 1
    ledcSetup(2, 5000, 8);      // Channel 2
    
    ledcAttachPin(_red_pin, 0);    // Attach red pin to channel 0
    ledcAttachPin(_green_pin, 1);  // Attach green pin to channel 1
    ledcAttachPin(_blue_pin, 2);   // Attach blue pin to channel 2
#else
    pinMode(_red_pin, OUTPUT);
    pinMode(_green_pin, OUTPUT);
    pinMode(_blue_pin, OUTPUT);
#endif
}

void RGBLed::off()
{
    color(0, 0, 0);
}

void RGBLed::brightness(int rgb[3], int brightness)
{
    _brightness = brightness;
    intensity(rgb[0], rgb[1], rgb[2], brightness);
}

void RGBLed::brightness(int red, int green, int blue, int brightness)
{
    _brightness = brightness;
    intensity(red, green, blue, brightness);
}

void RGBLed::brightness(int brightness)
{
    _brightness = brightness;
}

void RGBLed::intensity(int red, int green, int blue, int brightness)
{
    if (brightness > 100) brightness = 100;
    if (brightness < 0) brightness = 0;

    red = (red * brightness) / 100;
    green = (green * brightness) / 100;
    blue = (blue * brightness) / 100;
    color(red, green, blue);
}

void RGBLed::color(int red, int green, int blue)
{
    // Constrain values
    if (red < 0) red = 0;
    if (red > 255) red = 255;
    if (green < 0) green = 0;
    if (green > 255) green = 255;
    if (blue < 0) blue = 0;
    if (blue > 255) blue = 255;

    if (_common == COMMON_ANODE)
    {
#if defined(ESP32)
        ledcWrite(0, 255 - red);
        ledcWrite(1, 255 - green);
        ledcWrite(2, 255 - blue);
#else
        analogWrite(_red_pin, 255 - red);
        analogWrite(_green_pin, 255 - green);
        analogWrite(_blue_pin, 255 - blue);
#endif
    }
    else
    {
#if defined(ESP32)
        ledcWrite(0, red);
        ledcWrite(1, green);
        ledcWrite(2, blue);
#else
        analogWrite(_red_pin, red);
        analogWrite(_green_pin, green);
        analogWrite(_blue_pin, blue);
#endif
    }
}

void RGBLed::setColor(int rgb[3])
{
    intensity(rgb[0], rgb[1], rgb[2], _brightness);
}

void RGBLed::setColor(int red, int green, int blue)
{
	intensity(red, green, blue, _brightness);
}


void RGBLed::fadeOut(int rgb[3], int steps, int duration)
{
	fade(rgb[0], rgb[1], rgb[2], steps, duration, true);
}

void RGBLed::fadeOut(int red, int green, int blue, int steps, int duration)
{
	fade(red, green, blue, steps, duration, true);
}

void RGBLed::fadeIn(int rgb[3], int steps, int duration)
{
	fade(rgb[0], rgb[1], rgb[2], steps, duration, false);
}

void RGBLed::fadeIn(int red, int green, int blue, int steps, int duration)
{
	fade(red, green, blue, steps, duration, false);
}

void RGBLed::fade(int red, int green, int blue, int steps, int duration, bool out)
{
	if (duration < 0)
		duration = 0;
	if (steps < 1)
		steps = 1;

	if (out)
	{
		for (int i = steps; i >= 0; i--)
		{
			fade(red, green, blue, steps, duration, i * 255 / steps, i > 0);
		}
	}
	else
	{
		for (int i = 0; i <= steps; i++)
		{
			fade(red, green, blue, steps, duration, i * 255 / steps, i < steps);
		}
	}
}

void RGBLed::fade(int red, int green, int blue, int steps, int duration, int value, bool wait)
{
	float brightness = float(value) / 255.f;

	intensity(red * brightness, green * brightness, blue * brightness, _brightness);

	if (wait)
	{
		delay((unsigned long)(duration / steps));
	}
}

void RGBLed::crossFade(int rgbFrom[3], int rgbTo[3], int steps, int duration)
{
	crossFade(rgbFrom[0], rgbFrom[1], rgbFrom[2], rgbTo[0], rgbTo[1], rgbTo[2], steps, duration);
}

void RGBLed::crossFade(int fromRed, int fromGreen, int fromBlue, int toRed, int toGreen, int toBlue, int steps, int duration)
{
	// Determine per color change
	//   toValue - fromValue = deltaValue
	int deltaRed = toRed - fromRed;
	int deltaGreen = toGreen - fromGreen;
	int deltaBlue = toBlue - fromBlue;

	// Determine change per step per color
	//   deltaValue / steps
	//   Note: Forcing casting to floats to ensure smooth fades
	//         with large number of steps
	float changeRed = (float) deltaRed / (float) steps;
	float changeGreen = (float) deltaGreen / (float) steps;
	float changeBlue = (float) deltaBlue / (float) steps;

	// run through steps
	for (int i = 0; i <= steps; i++)
	{
		// Determine the new value for each color based on the step
		//  stepValue = fromValue + (changeValue * stepNumber)
		int stepRed = fromRed + (changeRed * i);
		int stepGreen = fromGreen + (changeGreen * i);
		int stepBlue = fromBlue + (changeBlue * i);

		// Make step change in color
		intensity(stepRed, stepGreen, stepBlue, _brightness);

		// delay based on total duration requested and number of steps
		if (i < steps)
		{
			delay((unsigned long)(duration / steps));
		}
	}
}

void RGBLed::gradient(int rgbFrom[3], int rgbTo[3], int step)
{
	gradient(rgbFrom[0], rgbFrom[1], rgbFrom[2], rgbTo[0], rgbTo[1], rgbTo[2], step);
}

void RGBLed::gradient(int fromRed, int fromGreen, int fromBlue, int toRed, int toGreen, int toBlue, int step)
{
	// Determine per color change
	//   toValue - fromValue = deltaValue
	int deltaRed = toRed - fromRed;
	int deltaGreen = toGreen - fromGreen;
	int deltaBlue = toBlue - fromBlue;

	// Determine change per step per color
	//   deltaValue / steps
	//   Note: Forcing casting to floats to ensure smooth fades
	//         with large number of steps
	float changeRed = (float) deltaRed / (float)255;
	float changeGreen = (float) deltaGreen / (float) 255;
	float changeBlue = (float) deltaBlue / (float) 255;

	
	// Determine the new value for each color based on the step
	//  stepValue = fromValue + (changeValue * stepNumber)
	int stepRed = fromRed + (changeRed * step);
	int stepGreen = fromGreen + (changeGreen * step);
	int stepBlue = fromBlue + (changeBlue * step);

	// Make step change in color
	intensity(stepRed, stepGreen, stepBlue, _brightness);
}

// Initialize pattern state
void RGBLed::callPattern(Pattern *pattern)
{
    _current_pattern = pattern;
    _pattern_active = true;
    _pattern_start_time = millis();
    _last_update_time = millis();
    _pattern_state = 0;
    _blink_count = 0;
    _fade_progress = 0.0f;
    _fade_direction = true;
}

// Stop the current pattern
void RGBLed::stopPattern()
{
    _pattern_active = false;
    _current_pattern = nullptr;
    off();
}

// Check if pattern is running
bool RGBLed::isPatternActive()
{
    return _pattern_active;
}

// Main update function - call this in loop()
void RGBLed::updatePattern(unsigned long now)
{
    if (!_pattern_active || _current_pattern == nullptr)
        return;

    switch (_current_pattern->type)
    {
    case PATTERN_STATIC:
        executeStaticPattern(now);
        break;
    case PATTERN_FADE:
        executeFadePattern(now);
        break;
    case PATTERN_BLINK:
        executeBlinkPattern(now);
        break;
    case PATTERN_CROSSFADE:
        executeCrossfadePattern(now);
        break;
    }
}

// Execute static color pattern
void RGBLed::executeStaticPattern(unsigned long now)
{
    StaticPattern *p = &_current_pattern->static_pattern;
    
    unsigned long elapsed = now - _pattern_start_time;
    
    if (_pattern_state == 0)
    {
        // Set the color
        intensity(p->r, p->g, p->b, _brightness);
        _pattern_state = 1;
    }
    
    if (elapsed >= p->duration)
    {
        if (_current_pattern->repeat)
        {
            // Restart pattern
            _pattern_start_time = now;
            _pattern_state = 0;
        }
        else
        {
            // Pattern complete
            stopPattern();
        }
    }
}

// Execute fade pattern
void RGBLed::executeFadePattern(unsigned long now)
{
    FadePattern *p = &_current_pattern->fade_pattern;
    unsigned long elapsed = now - _pattern_start_time;
    
    if (_pattern_state == 0)
    {
        // Fading in
        if (elapsed < p->time_on)
        {
            float progress = (float)elapsed / (float)p->time_on;
            
            uint8_t r = p->r_start + (p->r_end - p->r_start) * progress;
            uint8_t g = p->g_start + (p->g_end - p->g_start) * progress;
            uint8_t b = p->b_start + (p->b_end - p->b_start) * progress;
            
            intensity(r, g, b, _brightness);
        }
        else
        {
            // Fade complete, turn off
            intensity(p->r_end, p->g_end, p->b_end, _brightness);
            _pattern_state = 1;
            _last_update_time = now;
        }
    }
    else if (_pattern_state == 1)
    {
        // Off time
        if (now - _last_update_time >= p->time_off)
        {
            if (_current_pattern->repeat)
            {
                // Restart pattern
                _pattern_start_time = now;
                _pattern_state = 0;
            }
            else
            {
                off();
                stopPattern();
            }
        }
    }
}

// Execute blink pattern
void RGBLed::executeBlinkPattern(unsigned long now)
{
    BlinkPattern *p = &_current_pattern->blink_pattern;
    unsigned long elapsed = now - _last_update_time;
    
    if (_pattern_state == 0)
    {
        // LED ON
        intensity(p->r, p->g, p->b, _brightness);
        
        if (elapsed >= p->time_on)
        {
            _pattern_state = 1;
            _last_update_time = now;
        }
    }
    else if (_pattern_state == 1)
    {
        // LED OFF
        off();
        
        if (elapsed >= p->time_off)
        {
            _blink_count++;
            
            // Check if we've completed the requested blinks
            if (p->repeat_count > 0 && _blink_count >= p->repeat_count)
            {
                if (_current_pattern->repeat)
                {
                    // Restart entire pattern
                    _blink_count = 0;
                    _pattern_state = 0;
                    _last_update_time = now;
                }
                else
                {
                    stopPattern();
                }
            }
            else
            {
                _pattern_state = 0;
                _last_update_time = now;
            }
        }
    }
}

// Execute crossfade pattern
void RGBLed::executeCrossfadePattern(unsigned long now)
{
    CrossfadePattern *p = &_current_pattern->crossfade_pattern;
    unsigned long elapsed = now - _last_update_time;
    
    if (_pattern_state == 0)
    {
        // Fading from start to end
        if (elapsed < p->fade_duration)
        {
            float progress = (float)elapsed / (float)p->fade_duration;
            
            uint8_t r = p->r_from + (p->r_to - p->r_from) * progress;
            uint8_t g = p->g_from + (p->g_to - p->g_from) * progress;
            uint8_t b = p->b_from + (p->b_to - p->b_from) * progress;
            
            intensity(r, g, b, _brightness);
        }
        else
        {
            intensity(p->r_to, p->g_to, p->b_to, _brightness);
            _pattern_state = 1;
            _last_update_time = now;
        }
    }
    else if (_pattern_state == 1)
    {
        // Hold at end color
        if (elapsed >= p->hold_duration)
        {
            if (p->reverse)
            {
                // Start reverse fade
                _pattern_state = 2;
                _last_update_time = now;
            }
            else if (_current_pattern->repeat)
            {
                // Restart pattern
                _pattern_state = 0;
                _last_update_time = now;
            }
            else
            {
                stopPattern();
            }
        }
    }
    else if (_pattern_state == 2)
    {
        // Fading back from end to start
        if (elapsed < p->fade_duration)
        {
            float progress = (float)elapsed / (float)p->fade_duration;
            
            uint8_t r = p->r_to + (p->r_from - p->r_to) * progress;
            uint8_t g = p->g_to + (p->g_from - p->g_to) * progress;
            uint8_t b = p->b_to + (p->b_from - p->b_to) * progress;
            
            intensity(r, g, b, _brightness);
        }
        else
        {
            intensity(p->r_from, p->g_from, p->b_from, _brightness);
            _pattern_state = 3;
            _last_update_time = now;
        }
    }
    else if (_pattern_state == 3)
    {
        // Hold at start color
        if (elapsed >= p->hold_duration)
        {
            if (_current_pattern->repeat)
            {
                // Restart pattern
                _pattern_state = 0;
                _last_update_time = now;
            }
            else
            {
                stopPattern();
            }
        }
    }
}

// Helper function to create static pattern
Pattern RGBLed::createStaticPattern(uint8_t r, uint8_t g, uint8_t b, unsigned long duration, bool repeat)
{
    Pattern pattern;
    pattern.type = PATTERN_STATIC;
    pattern.static_pattern.r = r;
    pattern.static_pattern.g = g;
    pattern.static_pattern.b = b;
    pattern.static_pattern.duration = duration;
    pattern.repeat = repeat;
    return pattern;
}

// Helper function to create fade pattern
Pattern RGBLed::createFadePattern(uint8_t r_start, uint8_t g_start, uint8_t b_start,
                                   uint8_t r_end, uint8_t g_end, uint8_t b_end,
                                   unsigned long time_on, unsigned long time_off, bool repeat)
{
    Pattern pattern;
    pattern.type = PATTERN_FADE;
    pattern.fade_pattern.r_start = r_start;
    pattern.fade_pattern.g_start = g_start;
    pattern.fade_pattern.b_start = b_start;
    pattern.fade_pattern.r_end = r_end;
    pattern.fade_pattern.g_end = g_end;
    pattern.fade_pattern.b_end = b_end;
    pattern.fade_pattern.time_on = time_on;
    pattern.fade_pattern.time_off = time_off;
    pattern.repeat = repeat;
    return pattern;
}

// Helper function to create blink pattern
Pattern RGBLed::createBlinkPattern(uint8_t r, uint8_t g, uint8_t b,
                                    unsigned long time_on, unsigned long time_off,
                                    uint8_t repeat_count, bool repeat)
{
    Pattern pattern;
    pattern.type = PATTERN_BLINK;
    pattern.blink_pattern.r = r;
    pattern.blink_pattern.g = g;
    pattern.blink_pattern.b = b;
    pattern.blink_pattern.time_on = time_on;
    pattern.blink_pattern.time_off = time_off;
    pattern.blink_pattern.repeat_count = repeat_count;
    pattern.repeat = repeat;
    return pattern;
}

// Helper function to create crossfade pattern
Pattern RGBLed::createCrossfadePattern(uint8_t r_from, uint8_t g_from, uint8_t b_from,
                                        uint8_t r_to, uint8_t g_to, uint8_t b_to,
                                        unsigned long fade_duration, unsigned long hold_duration,
                                        bool reverse, bool repeat)
{
    Pattern pattern;
    pattern.type = PATTERN_CROSSFADE;
    pattern.crossfade_pattern.r_from = r_from;
    pattern.crossfade_pattern.g_from = g_from;
    pattern.crossfade_pattern.b_from = b_from;
    pattern.crossfade_pattern.r_to = r_to;
    pattern.crossfade_pattern.g_to = g_to;
    pattern.crossfade_pattern.b_to = b_to;
    pattern.crossfade_pattern.fade_duration = fade_duration;
    pattern.crossfade_pattern.hold_duration = hold_duration;
    pattern.crossfade_pattern.reverse = reverse;
    pattern.repeat = repeat;
    return pattern;
}