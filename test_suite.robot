*** Settings ***
Documentation   Integration test suite
Default Tags    test_system
Library         Process
Library         OperatingSystem
Library         ${CURDIR}/robot_framework/ModalityClientLibrary.py

Suite Setup     Test System Suite Setup
Suite Teardown  Test System Suite Teardown
Test Setup      Test System Test Setup
Test Teardown   Test System Test Teardown

*** Variables ***
${RUN_ID_SCRIPT}                ${CURDIR}/scripts/get_test_run_id.sh
${BUILD_SCRIPT}                 ${CURDIR}/scripts/build.sh
${START_SCRIPT}                 ${CURDIR}/scripts/start.sh
${STOP_SCRIPT}                  ${CURDIR}/scripts/stop_docker.sh
${KILLALL_SCRIPT}               ${CURDIR}/scripts/killall.sh
${REFLECTOR_CONFIG}             ${CURDIR}/config/reflector-config.toml

*** Keywords ***
Test System Suite Setup
    On Suite Setup                  ${SUITE_NAME}
    Build System

Test System Suite Teardown
    On Suite Teardown
    Terminate All Processes
    Run Command                     ${STOP_SCRIPT}

Test System Test Setup
    ${runid}                        Run Command         ${RUN_ID_SCRIPT}
    Set Environment Variable        MODALITY_RUN_ID     ${runid.stdout}
    On Test Setup                   ${TEST_NAME}

Test System Test Teardown
    Run Keyword If Test Failed
      ...   On Test Failed          ${TEST_NAME}
    Run Keyword If Test Passed
      ...   On Test Passed          ${TEST_NAME}
    On Test Teardown                ${TEST_NAME}

Run Command
    [Arguments]                     ${cmd_and_args}
    ${result} =                     Run Process         ${cmd_and_args}  shell=true
    IF                              ${result.rc} != 0
        Log To Console              Error running command: ${cmd_and_args}    console=yes
        Log To Console              ${result.stdout}    console=yes
        Log To Console              ${result.stderr}    console=yes
    END
    Should Be Equal As Integers     ${result.rc}        0
    RETURN                          ${result}

Build System
    Run Command                     ${BUILD_SCRIPT}

Start Reflector
    Start Process                   modality-reflector
    ...                                 run  --config  ${REFLECTOR_CONFIG}  --collector  lttng-live  --collector   trace-recorder-tcp
    ...                                 alias=reflector
    ...                                 stdout=/tmp/reflector.stdout
    ...                                 stderr=/tmp/reflector.stdout
    Run Process                     curl  --retry-max-time  30  --retry  10  --retry-connrefused  http://localhost:14188

Start System
    ${startup_nonce}                On Start Component          sensor
    Set Environment Variable        FREERTOS_STARTUP_NONCE      ${startup_nonce}
    ${run_id}                       Get Environment Variable    MODALITY_RUN_ID
    ${voltage_spike_enabled}        Evaluate                    ${run_id} % 2
    Set Environment Variable        VOLTAGE_SPIKE_ENABLED       ${voltage_spike_enabled}
    Run Command                     ${START_SCRIPT}

Stop System
    Run Command                     ${STOP_SCRIPT}
    Terminate Process               reflector
    Run Command                     ${KILLALL_SCRIPT}

Wait For Contact
    Run Command                     modality wait-until "contact@robot_chassis -> publish_message@robot_chassis AGGREGATE count() > 0" --deadline "30s"

Evaluate Specs
    Run Command                     modality workspace sync-indices
    Run Command                     conform spec eval --name rtos_resources
    Run Command                     conform spec eval --name system

*** Test Cases ***
Run Until Contact
    [Documentation]                 Run the sytem until the robot makes contact with the wall
    [Tags]                          gazebo freertos linux

    Start Reflector
    Start System
    Wait For Contact
    Stop System
    Evaluate Specs
