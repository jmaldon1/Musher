import musher
import os

TESTS_DIR_PATH = os.path.abspath(os.path.dirname(__file__))

def test_print_functional_message():
    # help(musher)
    musher.PrintFunctionalMessage("hi")


# def test_load_audio_file():
#     # print(test_dir_path)
#     abs_audio_file_path = os.path.join(TESTS_DIR_PATH, "audio_files", "CantinaBand3sec.wav")
#     file_data = musher.load_audio_file(abs_audio_file_path)

#     # with open("./tests/test_include/filedata.txt", "w") as f:
#     #     f.write("{")
#     #     for idx, val in enumerate(file_data):
#     #         f.write(str(val) + ", ")
#     #         if idx % 32 == 0:
#     #             f.write("\n")

#     #     f.write("}")

#     # print(t)
#     # musher.DecodeWav("hello")


# # def test_decode_with_func_call():
#     # musher.AcceptDecode("hello", musher.DecodeWav)

def test_decode_wav():
    abs_audio_file_path = os.path.join(TESTS_DIR_PATH, "..", "data", "audio_files", "mozart_c_major_30sec.wav")
    file_data = musher.load_audio_file(abs_audio_file_path)
    decoded_wav_dict = musher.decode_wav(file_data)
    print("\nsample_rate:", decoded_wav_dict["sample_rate"])
    print("bit_depth:", decoded_wav_dict["bit_depth"])
    print("channels:", decoded_wav_dict["channels"])
    print("mono:", decoded_wav_dict["mono"])
    print("stereo:", decoded_wav_dict["stereo"])
    print("length_in_seconds:", decoded_wav_dict["length_in_seconds"])
    print("file_type:", decoded_wav_dict["file_type"])
    print("avg_bitrate_kbps:", decoded_wav_dict["avg_bitrate_kbps"])
    # print(decoded_wav_dict)
