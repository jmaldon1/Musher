Windowing
==================

.. doxygenfunction:: BlackmanHarris
   :project: musher


HPCP
==================
.. doxygenfunction:: HPCP(const std::vector<double> &frequencies, const std::vector<double> &magnitudes, unsigned int size = 12, double reference_frequency = 440.0, unsigned int harmonics = 0, bool band_preset = true, double band_split_frequency = 500.0, double min_frequency = 40.0, double max_frequency = 5000.0, std::string _weight_type = "squared cosine", double window_size = 1.0, bool max_shifted = false, bool non_linear = false, std::string _normalized = "unit max")
   :project: musher
.. doxygenfunction:: HPCP(const std::vector<std::tuple<double, double>> &peaks, unsigned int size = 12, double reference_frequency = 440.0, unsigned int harmonics = 0, bool band_preset = true, double band_split_frequency = 500.0, double min_frequency = 40.0, double max_frequency = 5000.0, std::string _weight_type = "squared cosine", double window_size = 1.0, bool max_shifted = false, bool non_linear = false, std::string _normalized = "unit max")
   :project: musher