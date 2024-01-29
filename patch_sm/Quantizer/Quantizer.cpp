#include <array>

#include "daisy_patch_sm.h"
#include "daisysp.h"

using namespace daisy;
using namespace patch_sm;
using namespace daisysp;

/** Hardware object for the patch_sm */
DaisyPatchSM patch;

/* Eurorack uses volt per octave scaling so mapping volts to Hz is
conveniently linear. */
constexpr float semitone = 1.f / 12.f;

// ** Built-in Scales **
// Here we represent the scale as a 16-bit int, where the notes in the
// 12-tone scale are listed as active or inactive from right to left. The
// least significant bit denotes the root of the scale, while the most
// significant denotes the major 7th.
// Western modes
constexpr uint16_t twelve_tone =0b111111111111;
constexpr int16_t ionian =      0b101010110101;
constexpr int16_t dorian =      0b011010101101;
constexpr int16_t aeolian =     0b010110101101;
constexpr int16_t hirajoshi =   0b000110001101;
constexpr int16_t whole_tone =  0b010101010101;
constexpr int16_t maj_7 =       0b100010010001;
constexpr int16_t min_7 =       0b010010001001;
// TODO: Add insen, iwato, pelog

// Find the minimum distance we can go from our note value to an index
// of an active note. This wraps, so 11 is only 1 away from zero.
// Round the note and take as first index. Then take both notes 1 index
// away. If only one, take it. If both, compare distance. If neither,
// set distance to 2 and repeat.
int8_t find_distance(const uint16_t scale, const float note)
{
    const uint8_t index = round(note);
    int8_t distance = 1;
    for (; distance < 13; ++distance)
    {
        const bool positive = scale >> ((index + distance) % 12) & 1;
        const bool negative = scale >> ((index - distance) % 12) & 1;
        //if (positive) break;
        //if (negative) { distance = -distance; break; }
        if (!(positive || negative)) continue;
        if (positive && negative) { 
            const uint8_t d = abs(note - (index + distance));
            const uint8_t e = abs(note - (index - distance));
            return d < e ? distance : -distance;
        }
        return positive ? distance : -distance;
    }
    // This would happen if the scale was 0x0;
    return 0;
};

int main(void)
{
    // TODO: Allow user to select or define the scale
    const uint16_t scale = whole_tone;

    /** Initialize the patch_sm hardware object */
    patch.Init();

    /** Loop forever */
    while(1)
    {

        patch.ProcessAllControls();
        bool state = patch.gate_in_1.State();
        patch.SetLed(state);
        // Only update CV output when a gate signal is received.
        // TODO: Always quantize if no input is connected to gate_in_1
        if (!state) continue;
        // Only quntize if there's a value to quantize to
        if (scale == 0) continue; 

        // The voltage range for this CV input is -5v to 5v. However the return
        // value from GetAdcValue is normalized to -1v to 1v. It's scaled by
        // AnalogControl. I asked about this here:
        // https://discord.com/channels/1037767234803740694/1039589722831474830/1201020358078234635
        const float voct_cv = patch.GetAdcValue(CV_5);
        // This mapping restores the CV input value to a five octave range. The
        // range of the CV output is 0-5v
        float voct = fmap(voct_cv, 0.f, 5.f);

        /* TODO: Add offset knob to help with input CV calibration */

        const float octave = floorf(voct / 1.f);
        const float note = (voct - octave) / semitone;
        
        // If we can quantize the current input to the nearest semitone, we're
        // all done!
        const uint8_t nearest_index = round(note);
        if (scale >> (nearest_index % 12) & 1)
        {
            patch.WriteCvOut(CV_OUT_1, octave + roundf(note) * semitone);
            continue;
        }
        // Use find disance to find the nearest active note in the scale. This
        // can be positive or negative. For example, if the note value is 3.25,
        // the nearest active semitone could be 5 (distance will be 2), or it
        // could be 2 if 2 is active (distance will be -1).
        const int8_t distance = find_distance(scale, note);
        const float out = octave + (roundf(note) + distance) * semitone;
        patch.WriteCvOut(CV_OUT_1, out);
    }
}
