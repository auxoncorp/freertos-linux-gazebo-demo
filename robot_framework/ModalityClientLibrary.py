from modality_client import ModalityClient
import os

__version__ = '0.1.0'

class ModalityClientLibrary(object):
    ROBOT_LIBRARY_VERSION = __version__
    ROBOT_LIBRARY_SCOPE = 'GLOBAL'

    def __init__(self):
        self.client = ModalityClient(["module='test-framework'", "execution_context='demo'"]);

    def on_suite_setup(self, suite_name):
        self.client.on_suite_setup(suite_name)

    def on_suite_teardown(self):
        self.client.on_suite_teardown()

    def on_test_setup(self, test_name):
        self.client.on_test_setup(test_name)

    def on_test_teardown(self, test_name):
        self.client.on_test_teardown(test_name)

    def on_test_passed(self, test_name):
        self.client.on_test_passed(test_name)

    def on_test_failed(self, test_name):
        self.client.on_test_failed(test_name)

    def on_start_component(self, component_name):
        return self.client.start_component(component_name)
