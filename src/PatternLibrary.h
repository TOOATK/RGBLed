/*
 * PatternLibrary.h
 * Predefined LED patterns for easy reuse
 * Place this file in: lib/RGBLed/src/PatternLibrary.h
 */

#ifndef PATTERN_LIBRARY_H
#define PATTERN_LIBRARY_H

#include "RGBLed.h"

class PatternLibrary
{
public:
    // ===== STATUS INDICATORS =====
    
    // Idle state - slow blue breathing
    static Pattern idle()
    {
        return RGBLed::createFadePattern(
            0, 0, 50,      // Start: dim blue
            0, 0, 255,     // End: bright blue
            2000,          // Fade time: 2s
            1000,          // Off time: 1s
            true           // Repeat
        );
    }
    
    // Processing/busy - fast yellow blink
    static Pattern processing()
    {
        return RGBLed::createBlinkPattern(
            255, 255, 0,   // Yellow
            100,           // On: 100ms
            100,           // Off: 100ms
            0,             // Infinite repeats
            true           // Repeat pattern
        );
    }
    
    // Error state - red pulse
    static Pattern error()
    {
        return RGBLed::createCrossfadePattern(
            50, 0, 0,      // From: dim red
            255, 0, 0,     // To: bright red
            300,           // Fade: 300ms
            300,           // Hold: 300ms
            true,          // Reverse
            true           // Repeat
        );
    }
    
    // Success - solid green for 2 seconds
    static Pattern success()
    {
        return RGBLed::createStaticPattern(
            0, 255, 0,     // Green
            2000,          // Duration: 2s
            false          // No repeat
        );
    }
    
    // Warning - orange blink
    static Pattern warning()
    {
        return RGBLed::createBlinkPattern(
            255, 165, 0,   // Orange
            500,           // On: 500ms
            500,           // Off: 500ms
            0,             // Infinite
            true           // Repeat
        );
    }
    
    // ===== NOTIFICATION PATTERNS =====
    
    // New message - quick blue flash
    static Pattern notification()
    {
        return RGBLed::createBlinkPattern(
            0, 100, 255,   // Light blue
            150,           // On: 150ms
            150,           // Off: 150ms
            3,             // Flash 3 times
            false          // Don't repeat
        );
    }
    
    // Attention needed - slow red blink
    static Pattern attention()
    {
        return RGBLed::createBlinkPattern(
            255, 0, 0,     // Red
            800,           // On: 800ms
            800,           // Off: 800ms
            0,             // Infinite
            true           // Repeat
        );
    }
    
    // ===== SYSTEM STATES =====
    
    // Startup sequence - cyan to white fade
    static Pattern startup()
    {
        return RGBLed::createFadePattern(
            0, 255, 255,   // Start: cyan
            255, 255, 255, // End: white
            1500,          // Fade: 1.5s
            500,           // Hold: 0.5s
            false          // No repeat
        );
    }
    
    // Shutdown - white to off fade
    static Pattern shutdown()
    {
        return RGBLed::createFadePattern(
            255, 255, 255, // Start: white
            0, 0, 0,       // End: off
            1000,          // Fade: 1s
            0,             // No hold
            false          // No repeat
        );
    }
    
    // Standby - dim yellow pulse
    static Pattern standby()
    {
        return RGBLed::createCrossfadePattern(
            20, 20, 0,     // From: very dim yellow
            100, 100, 0,   // To: dim yellow
            2000,          // Fade: 2s
            1000,          // Hold: 1s
            true,          // Reverse
            true           // Repeat
        );
    }
    
    // ===== CHARGING/BATTERY PATTERNS =====
    
    // Charging - green pulse
    static Pattern charging()
    {
        return RGBLed::createCrossfadePattern(
            0, 50, 0,      // From: dim green
            0, 255, 0,     // To: bright green
            1000,          // Fade: 1s
            500,           // Hold: 0.5s
            true,          // Reverse
            true           // Repeat
        );
    }
    
    // Low battery - red slow blink
    static Pattern lowBattery()
    {
        return RGBLed::createBlinkPattern(
            255, 0, 0,     // Red
            1000,          // On: 1s
            2000,          // Off: 2s
            0,             // Infinite
            true           // Repeat
        );
    }
    
    // Full charge - solid green
    static Pattern fullyCharged()
    {
        return RGBLed::createStaticPattern(
            0, 255, 0,     // Green
            5000,          // Duration: 5s
            false          // No repeat
        );
    }
    
    // ===== DECORATIVE PATTERNS =====
    
    // Rainbow cycle (red to green)
    static Pattern rainbow()
    {
        return RGBLed::createCrossfadePattern(
            255, 0, 0,     // Red
            0, 255, 0,     // Green
            2000,          // Fade: 2s
            500,           // Hold: 0.5s
            true,          // Reverse
            true           // Repeat
        );
    }
    
    // Breathing white
    static Pattern breathingWhite()
    {
        return RGBLed::createFadePattern(
            10, 10, 10,    // Start: very dim
            255, 255, 255, // End: bright white
            2000,          // Fade: 2s
            1000,          // Off: 1s
            true           // Repeat
        );
    }
    
    // Party mode - fast multi-color
    static Pattern party()
    {
        return RGBLed::createCrossfadePattern(
            255, 0, 255,   // Magenta
            255, 255, 0,   // Yellow
            300,           // Fast fade: 300ms
            200,           // Short hold: 200ms
            true,          // Reverse
            true           // Repeat
        );
    }
    
    // ===== TEMPERATURE INDICATORS =====
    
    // Cool - blue static
    static Pattern cool()
    {
        return RGBLed::createStaticPattern(
            0, 100, 255,   // Light blue
            0,             // Infinite
            true           // Keep on
        );
    }
    
    // Warm - orange/red fade
    static Pattern warm()
    {
        return RGBLed::createCrossfadePattern(
            255, 165, 0,   // Orange
            255, 0, 0,     // Red
            1500,          // Fade: 1.5s
            500,           // Hold: 0.5s
            true,          // Reverse
            true           // Repeat
        );
    }
    
    // Overheat - fast red blink
    static Pattern overheat()
    {
        return RGBLed::createBlinkPattern(
            255, 0, 0,     // Bright red
            200,           // On: 200ms
            200,           // Off: 200ms
            0,             // Infinite
            true           // Repeat
        );
    }
    
    // ===== CONNECTIVITY PATTERNS =====
    
    // Connecting - cyan pulse
    static Pattern connecting()
    {
        return RGBLed::createCrossfadePattern(
            0, 100, 100,   // Dim cyan
            0, 255, 255,   // Bright cyan
            500,           // Fast fade: 500ms
            300,           // Short hold: 300ms
            true,          // Reverse
            true           // Repeat
        );
    }
    
    // Connected - solid green
    static Pattern connected()
    {
        return RGBLed::createStaticPattern(
            0, 255, 0,     // Green
            0,             // Infinite
            true           // Keep on
        );
    }
    
    // Disconnected - red blink
    static Pattern disconnected()
    {
        return RGBLed::createBlinkPattern(
            255, 0, 0,     // Red
            300,           // On: 300ms
            1700,          // Off: 1.7s
            0,             // Infinite
            true           // Repeat
        );
    }
    
    // ===== OFF STATE =====
    
    // Turn off
    static Pattern off()
    {
        return RGBLed::createStaticPattern(
            0, 0, 0,       // Off
            0,             // Infinite
            true           // Keep off
        );
    }
};

#endif // PATTERN_LIBRARY_H