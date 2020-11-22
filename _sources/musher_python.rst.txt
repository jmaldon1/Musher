.. musher package
.. ==============

.. .. Submodules
.. .. ----------

.. Musher module
.. ----------------------------\


Musher Module
=============

.. automodule:: musher.musher_python
.. currentmodule:: musher


Audio Decoders
--------------

.. autofunction:: load_audio_file
.. autofunction:: decode_wav_from_data
.. autofunction:: decode_wav_from_file
.. autofunction:: decode_mp3_from_file 


.. _notes_label:

NOTES
~~~~~~

.. code-block:: python

   audio_decoded = musher.decode_wav_from_file(abs_audio_file_path)

   # Will be audio channel 1.
   channel_one = audio_decoded["normalized_samples"][0]

   # Will be audio channel 2 OR will not exist if mono audio (audio_decoded["mono"]==False).
   channel_two_maybe = audio_decoded["normalized_samples"][1]

Framecutter
-----------

.. autoclass:: Framecutter
   :special-members:

HPCP
----

.. autofunction:: hpcp
.. autofunction:: hpcp_from_peaks

Mono Mixer
----------

.. autofunction:: mono_mixer

Key
---

.. autofunction:: estimate_key
.. autofunction:: detect_key

Spectrum
--------

.. autofunction:: convert_to_frequency_spectrum

Spectral Peaks
--------------

.. autofunction:: spectral_peaks

Windowing
---------

.. autofunction:: square
.. autofunction:: blackmanharris
.. autofunction:: blackmanharris62dB
.. autofunction:: blackmanharris92dB
.. autofunction:: windowing


.. Musher module
.. =============

.. .. automodule:: musher.musher_python
..    :members:
..    :undoc-members:
..    :show-inheritance:

.. Module contents
.. ===============

.. .. automodule:: musher
..    :members:
..    :undoc-members:
..    :show-inheritance:
