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
        stage('Zipping Source Files') {
            steps {
                sshagent(['windowsvm-key']) {
                    sh """
                        echo "Zipping and transferring files..."
                        zip -r build.zip . -x "*.git/*"
                        scp -o StrictHostKeyChecking=no build.zip vboxuser@windowsvm:build.zip
                        rm build.zip
                    """
                }
            }
        }
        stage('Linting Files') {
            steps {
                sh 'cpplint *.cpp *.hpp'  // Pipeline will fail if cpplint exits with 1
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

        stage('Building Application') {
            steps {
                sshagent(['windowsvm-key']) {
                    sh """
                        ssh -o StrictHostKeyChecking=no vboxuser@windowsvm << EOF
                        echo "Starting build process..."
                        C:\\Windows\\System32\\cmd.exe /A /Q /K C:\\Qt\\6.8.2\\mingw_64\\bin\\qtenv2.bat
                        cmake -G Ninja -S C:\\Users\\vboxuser\\RR-Build -B C:\\Users\\vboxuser\\RR-App "-DCMAKE_BUILD_TYPE:STRING=Release"
                        cd C:\\Users\\vboxuser\\RR-App
                        ninja

                        echo "Packaging Dependencies..."
                        powershell -command "cp C:\\Users\\vboxuser\\RR-App\\Replica-Reaper.exe C:\\Users\\vboxuser\\RR-Executable\\Replica-Reaper.exe"
                        powershell -command "cp C:\\Users\\vboxuser\\RR-App\\UnitTest\\UnitTests.exe C:\\Users\\vboxuser\\RR-UnitTests\\UnitTests.exe"

                        cd C:\\Users\\vboxuser\\RR-Executable
                        windeployqt .
                        cd C:\\Users\\vboxuser\\RR-UnitTests
                        windeployqt .

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
        stage("Running Unit Tests"){
            steps{
                sshagent(['windowsvm-key']) {
                    sh """
                        ssh -o StrictHostKeyChecking=no vboxuser@windowsvm << EOF
                        echo "Running Unit Tests..."
                        cd C:\\Users\\vboxuser\\RR-UnitTests
                        C:\\Users\\vboxuser\\RR-UnitTests\\UnitTests.exe
                        exit %ERRORLEVEL%
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
        stage('Archiving'){
            steps{
               archiveArtifacts artifacts: 'RR-Executable-UnitTests.zip', followSymlinks: false, onlyIfSuccessful: true 
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