from song import *


def main():
    print("================================================================")
    print("This is the python wrapper to test the Song shared library")
    print("================================================================")

    first_song = song("My First Song", 6.2)
    second_song = song("My Second Song", 12)

    print(first_song.GetName() + " " + str(first_song.GetLength()))
    print(second_song.GetName() + " " + str(second_song.GetLength()))

    PrintFunctionalMessage("This is a functional message")


if __name__ == "__main__":
    main()
