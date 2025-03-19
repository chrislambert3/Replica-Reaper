pipeline {
    agent any

    options {
        retry(1) // Retry once if any stage fails
        //catchError(buildResult: 'FAILURE', stageResult: 'FAILURE') // Catch errors and set failure status
    }

    stages {
        
        stage('Setting Build Status on Github'){
            steps {
            setBuildStatus("Build ${BUILD_DISPLAY_NAME} Pending", "PENDING")
            }
        }
        stage('Extract & Setup on Windows VM') {
            steps {
                sshagent(['windowsvm-key']) {
                    sh """
                        ssh -o StrictHostKeyChecking=no vboxuser@windowsvm << EOF
                        echo "Extracting files and setting up directories..."
                        powershell -command "Remove-Item -Recurse -Force C:\\Users\\vboxuser\\RR-Build"
                        powershell -command "Expand-Archive C:\\Users\\vboxuser\\build.zip C:\\Users\\vboxuser\\RR-Build"
                        EOF
                    """
                }
            }
        }

        stage('Build & Deploy') {
            steps {
                sshagent(['windowsvm-key']) {
                    sh """
                        ssh -o StrictHostKeyChecking=no vboxuser@windowsvm << EOF
                        echo "Starting build process..."
                        C:\\Windows\\System32\\cmd.exe /A /Q /K C:\\Qt\\6.8.2\\mingw_64\\bin\\qtenv2.bat
                        cmake -G Ninja -S C:\\Users\\vboxuser\\RR-Build -B C:\\Users\\vboxuser\\RR-App "-DCMAKE_BUILD_TYPE:STRING=Release"
                        cd C:\\Users\\vboxuser\\RR-App
                        ninja

                        echo "Deploying artifacts..."
                        powershell -command "cp C:\\Users\\vboxuser\\RR-App\\Replica-Reaper.exe C:\\Users\\vboxuser\\RR-Executable\\Replica-Reaper.exe"
                        powershell -command "cp C:\\Users\\vboxuser\\RR-App\\UnitTest\\UnitTests.exe C:\\Users\\vboxuser\\RR-UnitTests\\UnitTests.exe"

                        echo "Running windeployqt..."
                        cd C:\\Users\\vboxuser\\RR-Executable
                        windeployqt .
                        cd C:\\Users\\vboxuser\\RR-UnitTests
                        windeployqt .
                        echo "Running Unit Tests..."
                        C:\\Users\\vboxuser\\RR-UnitTests\\UnitTests.exe

                        echo "Creating final archive..."
                        powershell -command "Compress-Archive -Path C:\\Users\\vboxuser\\RR-Executable, C:\\Users\\vboxuser\\RR-UnitTests -DestinationPath C:\\Users\\vboxuser\\RR-Executable-UnitTests.zip -Force"

                        echo "Cleaning up build directories..."
                        powershell -command "Remove-Item -Recurse -Force C:\\Users\\vboxuser\\RR-Build"
                        powershell -command "Remove-Item -Recurse -Force C:\\Users\\vboxuser\\RR-App"
                        powershell -command "New-Item -Path C:\\Users\\vboxuser\\RR-Build -ItemType Directory"
                        powershell -command "New-Item -Path C:\\Users\\vboxuser\\RR-App -ItemType Directory"
                        
                        exit
                        exit
                        EOF
                    """
                }
            }
        }

        stage("Copy Artifacts to Jenkins") {
            steps {
                sshagent(['windowsvm-key']) {
                    sh 'scp vboxuser@windowsvm:RR-Executable-UnitTests.zip .'
                }
            }
        }

        stage('Completion') {
            steps {
                echo "Build completed successfully."
            }
        }
    }
    post{
        success{
            setBuildStatus("Build ${BUILD_DISPLAY_NAME} Complete", "SUCCESS")
        }
        failure{
            setBuildStatus("Build ${BUILD_DISPLAY_NAME} Failed", "FAILURE")
        }
    }
}
void setBuildStatus(String message, String state) {
    step([
        $class: "GitHubCommitStatusSetter",
        reposSource: [$class: "ManuallyEnteredRepositorySource", url: "https://github.com/chrislambert3/Replica-Reaper.git"],
        contextSource: [$class: "ManuallyEnteredCommitContextSource", context: "ci/jenkins/build-test"],
        errorHandlers: [[$class: "ChangingBuildStatusErrorHandler", result: "UNSTABLE"]],
        statusResultSource: [ $class: "ConditionalStatusResultSource", results: [[$class: "AnyBuildResult", message: message, state: state]] ]
    ])
}
