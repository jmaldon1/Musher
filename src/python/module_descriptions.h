/**
 * @file module_descriptions.h
 * @author Joshua Maldonado
 * @brief Python module doc strings.
 *
 * Block strings in C++:
 * https://stackoverflow.com/a/5460235
 *
 * @version 0.1
 * @date 2020-11-15
 *
 * @copyright Copyright (c) 2020
 *
 */
#pragma once

const char* load_audio_file_description = R"(
  Load the data from an audio file.

  Args:
    file_path (str): File path to a .wav file.

  Returns:
    numpy.ndarray[numpy.uint8]: Audio file data.
)";

const char* decode_wav_from_data_description = R"(
  Decode a wav file.
 
  Example:

    >>> wav_decoded = musher.decode_wav_from_file(path_to_mp3_file)
    >>> print(wav_decoded)
    {
      'avg_bitrate_kbps': 1411,
      'bit_depth': 16,
      'channels': 2,
      'file_type': 'wav',
      'length_in_seconds': 30.0,
      'mono': False,
      'normalized_samples': array([
            [ 0., 0., 0., ..., -0.33203125, -0.32833862, -0.3274536 ],
            [ 0., 0., 0., ..., -0.29162598, -0.27130127, -0.25457764]
          ], dtype=float32),
      'sample_rate': 44100,
      'samples_per_channel': 1323000,
      'stereo': True
    }


  See :ref:`notes<notes_label>` for extra details.

  Args:
    file_data (List[int]): WAV file data.

  Returns:
    dict: .wav file information.
)";

const char* decode_wav_from_file_description = R"(
  Overloaded wrapper around DecodeWav that accepts a file path to a .wav file.

  See :func:`musher.decode_wav_from_data` for an example.

  See :ref:`notes<notes_label>` for extra details.

  Args:
    file_path (str): File path to a .wav file.

  Returns:
    dict: .wav file information.
)";

const char* decode_mp3_from_file_description = R"(
  Decode an mp3 file.

  Example

    >>> mp3_decoded = musher.decode_mp3_from_file(audio_file_data)
    >>> print(mp3_decoded)
    {
      'avg_bitrate_kbps': 1411,
      'channels': 2,
      'file_type': 'mp3',
      'length_in_seconds': 30.0,
      'mono': False,
      'normalized_samples': array([
            [ 0., 0., 0., ..., -0.33203125, -0.32833862, -0.3274536 ],
            [ 0., 0., 0., ..., -0.29162598, -0.27130127, -0.25457764]
          ], dtype=float32),
      'sample_rate': 44100,
      'samples_per_channel': 1323000,
      'stereo': True
    }

  See :ref:`notes<notes_label>` for extra details.

  Args:
    file_path (str): File path to a .mp3 file.

  Returns:
    dict: .mp3 file information.
)";

const char* mono_mixer_description = R"(
  Downmixes the signal into a single channel given a stereo signal.

  If the signal was already a monoaural, it is left unchanged.

  Args:
    input (List[List[float]]): Stereo or mono audio signal

  Returns:
    numpy.ndarray[numpy.float64]: Downmixed audio signal
)";

const char* framecutter_description = R"(
  This class is an iterator.

  Examples:
    Iterate over the class like so:

    >>> buffer = [1., 2., 3., 4., 5.]
    >>> framecutter = musher.Framecutter(buffer, 3, 2, True, False, 0.)
    >>> for frame in framecutter:
    ...    print(frame)
    ...
    [0.0, 0.0, 1.0]
    [1.0, 2.0, 3.0]
    [3.0, 4.0, 5.0]
    [5.0, 0.0, 0.0]
)";

const char* framecutter_init_description = R"(
  Construct a new Framecutter object

  Args:
    buffer (List[float]): Buffer from which to read data.
    frame_size (int, optional): Output frame size. Defaults to 1024.
    hop_size (int, optional): Hop size between frames. Defaults to 512.
    start_from_center (bool, optional): If true start from the center of the buffer (zero-centered at -frameSize/2) or
      if false the first frame at time 0 (centered at frameSize/2). Defaults to True.
    last_frame_to_end_of_file (bool, optional): Whether the beginning of the last frame should reach the end of file. Only
      applicable if start_from_center is false. Defaults to False.
    valid_frame_threshold_ratio (List[float], optional): frames smaller than this ratio will be discarded, those larger will be
      zero-padded to a full frame. (i.e. a value of 0 will never discard frames and a value of 1 will only keep frames
      that are of length 'frameSize'). Defaults to 0.0.

  Returns:
    List[float]: Cut frame.
)";

const char* windowing_description = R"(
  Applies windowing to an audio signal.
  
  It optionally applies zero-phase windowing and optionally adds zero-padding. The resulting windowed frame size is
  equal to the incoming frame size plus the number of padded zeros. By default, the available windows are normalized
  (to have an area of 1) and then scaled by a factor of 2.

  References:

  [1] F. J. Harris, On the use of windows for harmonic analysis with the discrete Fourier transform,
  Proceedings of the IEEE, vol. 66, no. 1, pp. 51-83, Jan. 1978

  [2] Window function - Wikipedia, the free encyclopedia, http://en.wikipedia.org/wiki/Window_function

  Args:
    audio_frame (List[float]): Input audio frame.
    window_type_func (Callable[[List[float]], List[float]], optional): The window type function. Examples: BlackmanHarris92dB, BlackmanHarris62dB...
      Defaults to BlackmanHarris92dB.
    zero_padding_size (int, optional): Size of the zero-padding. Defaults to 0.
    zero_phase (bool, optional): Enables zero-phase windowing. Defaults to True.
    _normalize (bool, optional): Specify whether to normalize windows (to have an area of 1) and then scale by a factor of 2. Defaults to True.

  Returns:
    numpy.ndarray[numpy.float64]: Windowed audio frame.
)";

const char* square_description = R"(
  Square windowing function.

  Args:
    window (List[float]): Audio signal window.

  Returns:
    numpy.ndarray[numpy.float64]: Square window.
)";

const char* blackmanharris_description = R"(
  Blackmanharris windowing algorithm.

  Window functions help control spectral leakage when doing Fourier Analysis.

  Args:
    window (List[float]): Audio signal window.
    a0 (float): Constant a0.
    a1 (float): Constant a1.
    a2 (float): Constant a2.
    a3 (float): Constant a3.

  Returns:
    numpy.ndarray[numpy.float64]: BlackmanHarris window.
)";

const char* blackmanharris62dB_description = R"(
  Blackmanharris62db windowing algorithm.

  Args:
    window (List[float]): Audio signal window.

  Returns:
    numpy.ndarray[numpy.float64]: Blackmanharris62db window.
)";

const char* blackmanharris92dB_description = R"(
  Blackmanharris92db windowing algorithm.

  Args:
    window (List[float]): Audio signal window.

  Returns:
    numpy.ndarray[numpy.float64]: Blackmanharris92db window.
)";

const char* convert_to_frequency_spectrum_description = R"(
  Computes the frequency spectrum of an array of Reals.

  The resulting spectrum has a size which is half the size of the input array plus one.
  Bins contain raw (linear) magnitude values.

  Args:
    frame (List[float]): Input audio frame.

  Returns:
    numpy.ndarray[numpy.float64]: Frequency spectrum of the input audio signal.
)";

const char* peak_detect_description = R"(
  Computes the frequency spectrum of an array of Reals.

  The resulting spectrum has a size which is half the size of the input array plus one.
  Bins contain raw (linear) magnitude values.

  Args:
    inp (List[float]): Input vector.
    threshold (float, optional): Peaks below this given threshold are not outputted. Defaults to -1000.0.
    interpolate (bool, optional): Enables interpolation. Defaults to True.
    sort_by (str, optional): Ordering type of the outputted peaks (ascending by position
      or descending by height). Defaults to 'position'.
    max_num_peaks (int, optional): Maximum number of returned peaks (set to 0 to return all peaks). Defaults to 0.
    range (float, optional): Input range. Defaults to 0.0.
    min_pos (int, optional): Maximum frequency (position) of the range to evaluate [Hz]. Defaults to 0.
    max_pos (int, optional): Minimum frequency (position) of the range to evaluate [Hz]. Defaults to 0.

  Returns:
    musher.peaks: List of peaks, each peak being a tuple (positions, heights).
)";

const char* spectral_peaks_description = R"(
  Extracts peaks from a spectrum.

  It is important to note that the peak algorithm is independent of an input that is linear or in dB, so one has to
  adapt the threshold to fit with the type of data fed to it. The algorithm relies on PeakDetect algorithm which is
  run with parabolic interpolation [1]. The exactness of the peak-searching depends heavily on the windowing type. It
  gives best results with dB input, a blackman-harris 92dB window and interpolation set to true. According to [1],
  spectral peak frequencies tend to be about twice as accurate when dB magnitude is used rather than just linear
  magnitude. For further information about the peak detection, see the description of the PeakDetection algorithm.

  References:

  [1] Peak Detection, http://ccrma.stanford.edu/~jos/parshl/Peak_Detection_Steps_3.html

  Args:
    input_spectrum (List[float]): Input spectrum.
    threshold (float, optional): Peaks below this given threshold are not outputted. Defaults to -1000.0.
    sort_by (str, optional): Ordering type of the outputted peaks (ascending by frequency (position)
      or descending by magnitude (height)). Defaults to 'position'.
    max_num_peaks (int, optional): Maximum number of returned peaks (set to 0 to return all peaks). Defaults to 100.
    sample_rate (float, optional): Sampling rate of the audio signal [Hz]. Defaults to 44100.0.
    min_pos (int, optional): Maximum frequency (position) of the range to evaluate [Hz]. Defaults to 0.
    max_pos (int, optional): Minimum frequency (position) of the range to evaluate [Hz]. Defaults to 0.

  Returns:
    musher.peaks: List of spectral peaks, each peak being a tuple (frequency, magnitude).
)";

const char* hpcp_description = R"(
  Computes a Harmonic Pitch Class Profile (HPCP) from the spectral peaks of a signal.

  HPCP is a k*12 dimensional list which represents the intensities of the twelve (k==1) semitone pitch classes
  (corresponsing to notes from A to G#), or subdivisions of these (k>1).

  Args:
    frequencies (List[float]): Frequencies (positions) of the spectral peaks [Hz].
    magnitudes (List[float]): Magnitudes (heights) of the spectral peaks.
    size (int, optional): Size of the output HPCP (must be a positive nonzero multiple of 12). Defaults to 12.
    reference_frequency (float, optional): Reference frequency for semitone index calculation, corresponding to A3 [Hz]. Defaults to 440.0.
    harmonics (int, optional): Number of harmonics for frequency contribution, 0 indicates exclusive fundamental frequency
      contribution. Defaults to 0.
    band_preset (bool, optional): Enables whether to use a band preset. Defaults to True.
    band_split_frequency (float, optional): Split frequency for low and high bands, not used if bandPreset is false [Hz]. Defaults to 500.0.
    min_frequency (float, optional): Minimum frequency that contributes to the HPCP [Hz] (the difference between the min and split
      frequencies must not be less than 200.0 Hz). Defaults to 40.0.
    max_frequency (float, optional): Maximum frequency that contributes to the HPCP [Hz] (the difference between the max and split
      frequencies must not be less than 200.0 Hz). Defaults to 5000.0.
    _weight_type (str, optional): Type of weighting function for determining frequency contribution. Defaults to 'squared cosine'.
    window_size (float, optional): Size, in semitones, of the window used for the weighting. Defaults to 1.0.
    max_shifted (bool, optional): Whether to shift the HPCP list so that the maximum peak is at index 0. Defaults to False.
    non_linear (bool, optional): Apply non-linear post-processing to the output (use with _normalized='unit max'). Boosts values close
      to 1, decreases values close to 0. Defaults to False.
    _normalized (str, optional): Whether to normalize the HPCP list. Defaults to 'unit max'.

  Returns:
    numpy.ndarray[numpy.float64]: Resulting harmonic pitch class profile.
)";

const char* hpcp_from_peaks_description = R"(
  Overloaded function for HPCP that accepts a list of peaks.

  Refer to original HPCP function for more details.

  Args:
    peaks (musher.peaks): list of spectral peaks, each peak being a tuple (frequency, magnitude).
    size (int, optional): Size of the output HPCP (must be a positive nonzero multiple of 12). Defaults to 12.
    reference_frequency (float, optional): Reference frequency for semitone index calculation, corresponding to A3 [Hz]. Defaults to 440.0.
    harmonics (int, optional): Number of harmonics for frequency contribution, 0 indicates exclusive fundamental frequency
      contribution. Defaults to 0.
    band_preset (bool, optional): Enables whether to use a band preset. Defaults to True.
    band_split_frequency (float, optional): Split frequency for low and high bands, not used if bandPreset is false [Hz]. Defaults to 500.0.
    min_frequency (float, optional): Minimum frequency that contributes to the HPCP [Hz] (the difference between the min and split
      frequencies must not be less than 200.0 Hz). Defaults to 40.0.
    max_frequency (float, optional): Maximum frequency that contributes to the HPCP [Hz] (the difference between the max and split
      frequencies must not be less than 200.0 Hz). Defaults to 5000.0.
    _weight_type (str, optional): Type of weighting function for determining frequency contribution. Defaults to 'squared cosine'.
    window_size (float, optional): Size, in semitones, of the window used for the weighting. Defaults to 1.0.
    max_shifted (bool, optional): Whether to shift the HPCP list so that the maximum peak is at index 0. Defaults to False.
    non_linear (bool, optional): Apply non-linear post-processing to the output (use with _normalized='unit max'). Boosts values close
      to 1, decreases values close to 0. Defaults to False.
    _normalized (str, optional): Whether to normalize the HPCP list. Defaults to 'unit max'.

  Returns:
    numpy.ndarray[numpy.float64]: Resulting harmonic pitch class profile.
)";

const char* estimate_key_description = R"(
  Computes key estimate given a pitch class profile (HPCP).

  Args:
    pcp (List[float]): The input pitch class profile.
    use_polphony (bool, optional): Enables the use of polyphonic profiles to define key profiles (this includes the contributions
      from triads as well as pitch harmonics). Defaults to True.
    use_three_chords (bool, optional): Consider only the 3 main triad chords of the key (T, D, SD) to build the polyphonic profiles. Defaults to True.
    num_harmonics (int, optional): Number of harmonics that should contribute to the polyphonic profile (1 only considers the
      fundamental harmonic). Defaults to 4.
    slope (float, optional): Value of the slope of the exponential harmonic contribution to the polyphonic profile. Defaults to 0.6.
    profile_type (str, optional): The type of polyphic profile to use for correlation calculation. Defaults to 'Bgate'.
    use_maj_min (bool, optional): Use a third profile called 'majmin' for ambiguous tracks. Only available for the edma, bgate
      and braw profiles. Defaults to False.

  Returns:
    KeyOutput: Details of key estimate.
)";

const char* detect_key_description = R"(
  Computes key estimate given normalized samples.

  Args:
    normalized_samples (List[List[float]]): The input pitch class profile.
    sample_rate (float, optional): Sampling rate of the audio signal [Hz]. Defaults to 44100.0.
    profile_type (str, optional): The type of polyphic profile to use for correlation calculation. Defaults to 'Bgate'.
    use_polphony (bool, optional): Enables the use of polyphonic profiles to define key profiles (this includes the contributions
      from triads as well as pitch harmonics). Defaults to True.
    use_three_chords (bool, optional): Consider only the 3 main triad chords of the key (T, D, SD) to build the polyphonic profiles. Defaults to True.
    num_harmonics (int, optional): Number of harmonics that should contribute to the polyphonic profile (1 only considers the
      fundamental harmonic). Defaults to 4.
    slope (float, optional): Value of the slope of the exponential harmonic contribution to the polyphonic profile. Defaults to 0.6.
    use_maj_min (bool, optional): Use a third profile called 'majmin' for ambiguous tracks. Only available for the edma, bgate
      and braw profiles. Defaults to False.
    pcp_size (int, optional): Number of array elements used to represent a semitone times 12. Defaults to 36.
    frame_size (int, optional): Output frame size of framecutter. Defaults to 4096.
    hop_size (int, optional): Hop size between frames of framecutter. Defaults to 512.
    window_type_func (Callable[[List[float]], List[float]], optional): The window type function.
      Examples: BlackmanHarris92dB, BlackmanHarris62dB... Defaults to BlackmanHarris62dB.
    max_num_peaks (int, optional): Maximum number of returned peaks (set to 0 to return all peaks) for spectral peaks. Defaults to 100.
    window_size (float, optional): Size, in semitones, of the window used for the weighting for HPCP. Defaults to 0.5.

  Returns:
    KeyOutput: Details of key estimate.
)";
