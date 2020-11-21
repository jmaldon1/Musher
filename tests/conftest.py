import os
import pytest


@pytest.fixture()
def test_data_dir() -> str:
    """Test data directory that stores some files for testing purposes.

    Returns:
        str: Test data directory.
    """
    tests_dir_path = os.path.abspath(os.path.dirname(__file__))
    return os.path.join(tests_dir_path, "..", "data")
