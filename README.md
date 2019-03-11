# Cycloon


## Demodulator implementation notes

Input: integers from ADC at sample_frequency

Stage 1: Frequency detection. In groups of filter_length, apply cross correlation filters of frequency +/-f and +/-2f, where f is such that 1 period is 8 samples. Sum absolute values of cross correlations for f and 2f. Threshhold. Output as two integer streams at frequency sample_freqeuncy / filter_length

Stage 2: Syncing. A sliding (sum) window for symbol_length for symbol 0 (frequency f) is always running. SYMBOL START HAPPENS WHEN THE WINDOW SUM SHOWS CONSECUTIVE INCREASE FOR symbol_length POINTS. Pass input stream to demodulator.

Stage 3: Demodulation. In groups of SYMBOL_LENGTH, compute filter WINDOW_1 (sum of 2f sequence), WINDOW_2 (sum of f sequence), and compute ratios. Threshhold to determine bit 0 or 1 (maybe 5 dB = 3x). Presence of both or neither immediately passes control back to stage 2. Output BIT_STREAM.

Messages must be in groups of 8 bits (single unsigned char). EOM symbol should be 0x00. Max message length is 4096 chars.