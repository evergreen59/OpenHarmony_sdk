# Instructions
Autosign help you to sign app more efficiently


## Dependency
This signature script depends on Python 3.x.



## Configuration

Use your favorite text editor to open `autosign.config` to configure everything.
Below config must be replaced:
* config.signtool
* All password

### 1. For Windows

1. Run the `start_creat.bat` file in the terminal to generate certs.
2. Prepare your unsigned app and provision profile. Make sure correct config file location.
3. Run the `start_sign.bat` file in the terminal to start signing.
4. The generated artifacts will be saved in the folder you set in `config.targetDir` as default.

### 2. For Linux or MacOS


1. Add executable permission into `start_create.sh` and `start_sign.sh`.

   ```bash
   chmod a+x start_sign.sh
   
   chmod a+x start_create.sh
   ```

2. Run the `start_create.sh` file in the terminal to generate certs.

   ```bash
   ./start_create.sh
   ```
3. Prepare your unsigned app and provision profile. Make sure correct config file location.
4. Run the `start_sign.sh` file in the terminal to start signing.
   ```bash
   ./start_sign.sh
   ```
5. The generated artifacts will be saved in the folder you set in `config.targetDir` as default.
