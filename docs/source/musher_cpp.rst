Audio Decoders
==============

.. doxygenstruct:: musher::core::AudioDecoded
   :project: musher
   :members:
.. doxygenstruct:: musher::core::WavDecoded
   :project: musher
   :members:
.. doxygenstruct:: musher::core::Mp3Decoded
   :project: musher
   :members:
.. doxygenfunction:: LoadAudioFile
   :project: musher
.. doxygenfunction:: DecodeWav(const std::vector<uint8_t> &file_data)
   :project: musher
.. doxygenfunction:: DecodeWav(const std::string &file_path)
   :project: musher
.. doxygenfunction:: DecodeMp3
   :project: musher

FFT Convolve
============

.. doxygenfunction:: CenterVector
   :project: musher
.. doxygenfunction:: FFTConvolve
   :project: musher

Framecutter
===========

.. doxygenclass:: musher::core::Framecutter
   :project: musher
   :members:

HPCP
====

.. doxygenfunction:: ArgMax
   :project: musher
.. doxygenfunction:: NormalizeInPlace
   :project: musher
.. doxygenfunction:: NormalizeSumInPlace
   :project: musher
.. doxygenfunction:: AddContributionWithWeight
   :project: musher
.. doxygenfunction:: AddContributionWithoutWeight
   :project: musher
.. doxygenfunction:: AddContribution
   :project: musher
.. doxygenfunction:: InitHarmonicContributionTable
   :project: musher
.. doxygenfunction:: HPCP(const std::vector<double> &frequencies, const std::vector<double> &magnitudes, unsigned int size = 12, double reference_frequency = 440.0, unsigned int harmonics = 0, bool band_preset = true, double band_split_frequency = 500.0, double min_frequency = 40.0, double max_frequency = 5000.0, std::string _weight_type = "squared cosine", double window_size = 1.0, bool max_shifted = false, bool non_linear = false, std::string _normalized = "unit max")
   :project: musher
.. doxygenfunction:: HPCP(const std::vector<std::tuple<double, double>> &peaks, unsigned int size = 12, double reference_frequency = 440.0, unsigned int harmonics = 0, bool band_preset = true, double band_split_frequency = 500.0, double min_frequency = 40.0, double max_frequency = 5000.0, std::string _weight_type = "squared cosine", double window_size = 1.0, bool max_shifted = false, bool non_linear = false, std::string _normalized = "unit max")
   :project: musher

Key
===

.. doxygenfunction:: SelectKeyProfile
   :project: musher
.. doxygenfunction:: AddContributionHarmonics
   :project: musher
.. doxygenfunction:: AddMajorTriad
   :project: musher
.. doxygenfunction:: AddMinorTriad
   :project: musher
.. doxygenfunction:: ResizeProfileToPcpSize
   :project: musher
.. doxygenfunction:: StandardDeviation
   :project: musher
.. doxygenfunction:: EstimateKey
   :project: musher
.. doxygenfunction:: DetectKey
   :project: musher

Mono Mixer
==========

.. doxygenfunction:: MonoMixer
   :project: musher

Peak Detect
===========
.. doxygenfunction:: QuadraticInterpolation
   :project: musher
.. doxygenfunction:: PeakDetect
   :project: musher

Spectral Peaks
==============
.. doxygenfunction:: SpectralPeaks
   :project: musher

Spectrum
========
.. doxygenfunction:: Magnitude
   :project: musher
.. doxygenfunction:: NormFct(int inorm, size_t N)
   :project: musher
.. doxygenfunction:: NormFct(int inorm, const pocketfft::shape_t &shape, const pocketfft::shape_t &axes, size_t fct, int delta)
   :project: musher

.. doxygenfunction:: NextFastLen
   :project: musher
.. doxygenfunction:: ConvertToFrequencySpectrum
   :project: musher

Utilities
=========
.. doxygenfunction:: Uint8VectorToHexString
   :project: musher
.. doxygenfunction:: StrBetweenSQuotes
   :project: musher
.. doxygenfunction:: IsBigEndian
   :project: musher
.. doxygenfunction:: Deinterweave
   :project: musher
.. doxygenfunction:: Median
   :project: musher
.. doxygenfunction:: OnePoleFilter
   :project: musher

Windowing
=========

.. doxygenfunction:: Square
   :project: musher
.. doxygenfunction:: BlackmanHarris
   :project: musher
.. doxygenfunction:: BlackmanHarris62dB
   :project: musher
.. doxygenfunction:: BlackmanHarris92dB
   :project: musher
.. doxygenfunction:: Normalize
   :project: musher
.. doxygenfunction:: Windowing(const std::vector<double> &audio_frame, const std::function<std::vector<double>(const std::vector<double>&)> &window_type_func = BlackmanHarris62dB, unsigned zero_padding_size = 0, bool zero_phase = true, bool _normalize = true)
   :project: musher
