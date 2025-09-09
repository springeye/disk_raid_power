// DOM元素
const uploadArea = document.getElementById('uploadArea');
const firmwareFile = document.getElementById('firmwareFile');
const uploadBtn = document.getElementById('uploadBtn');
const progressContainer = document.getElementById('progressContainer');
const progress = document.getElementById('progress');
const progressText = document.getElementById('progressText');
const fileInfo = document.getElementById('fileInfo');
const fileName = document.getElementById('fileName');
const fileSize = document.getElementById('fileSize');
const fileType = document.getElementById('fileType');
const upgradeTypeRadios = document.querySelectorAll('input[name="upgradeType"]');
const mobileWarning = document.getElementById('mobileWarning');
const iosWarning = document.querySelector('.ios-warning');

// 全局变量
let selectedFile = null;
let currentUpgradeType = 'firmware';

// 初始化函数
function init() {
    setupEventListeners();
    updateFileAcceptAttribute();
    checkMobileDevice();
    resetUploadUI();
}

// 设置事件监听器
function setupEventListeners() {
    // 点击上传区域触发文件选择
    uploadArea.addEventListener('click', handleUploadAreaClick);

    // 拖放功能
    uploadArea.addEventListener('dragover', handleDragOver);
    uploadArea.addEventListener('dragleave', handleDragLeave);
    uploadArea.addEventListener('drop', handleDrop);

    // 文件选择处理
    firmwareFile.addEventListener('change', handleFileChange);

    // 上传按钮点击事件
    uploadBtn.addEventListener('click', handleUploadClick);

    // 升级类型选择
    upgradeTypeRadios.forEach(radio => {
        radio.addEventListener('change', handleUpgradeTypeChange);
    });
}

// 检测移动设备
function checkMobileDevice() {
    const isMobile = /Android|webOS|iPhone|iPad|iPod|BlackBerry|IEMobile|Opera Mini/i.test(navigator.userAgent);

    if (isMobile) {
        mobileWarning.style.display = 'flex';

        // 如果是iOS设备，添加额外提示
        if (/iPhone|iPad|iPod/i.test(navigator.userAgent)) {
            iosWarning.style.display = 'block';
        }
    }
}

// 更新文件接受属性
function updateFileAcceptAttribute() {
    const accept = currentUpgradeType === 'firmware' ? '.bin' : '.zip,.tar.gz,.tar';
    firmwareFile.setAttribute('accept', accept);
}

// 处理上传区域点击
function handleUploadAreaClick() {
    firmwareFile.click();
}

// 处理拖拽悬停
function handleDragOver(e) {
    e.preventDefault();
    uploadArea.classList.add('active');
}

// 处理拖拽离开
function handleDragLeave() {
    uploadArea.classList.remove('active');
}

// 处理拖拽放置
function handleDrop(e) {
    e.preventDefault();
    uploadArea.classList.remove('active');

    if (e.dataTransfer.files.length) {
        handleFileSelection(e.dataTransfer.files[0]);
    }
}

// 处理文件选择变化
function handleFileChange(e) {
    if (e.target.files.length) {
        handleFileSelection(e.target.files[0]);
    }
}

// 处理升级类型变化
function handleUpgradeTypeChange(e) {
    currentUpgradeType = e.target.value;
    updateFileAcceptAttribute();

    // 如果已经有选中的文件，重新验证
    if (selectedFile) {
        handleFileSelection(selectedFile);
    }
}

// 处理文件选择
function handleFileSelection(file) {
    const isValid = validateFile(file);
    if (!isValid) return;

    selectedFile = file;
    updateFileInfo(file);
    uploadBtn.disabled = false;
}

// 验证文件
function validateFile(file) {
    const extension = file.name.split('.').pop().toLowerCase();

    if (currentUpgradeType === 'firmware') {
        if (extension !== 'bin') {
            showError('请选择有效的.bin固件文件');
            return false;
        }
    } else {
        const validExtensions = ['zip', 'tar', 'gz'];
        if (!validExtensions.includes(extension)) {
            showError('请选择有效的资源文件(.zip, .tar.gz, .tar)');
            return false;
        }
    }

    // 文件大小限制（可选）
    const maxSize = currentUpgradeType === 'firmware' ? 50 * 1024 * 1024 : 100 * 1024 * 1024; // 50MB for firmware, 100MB for resources
    if (file.size > maxSize) {
        showError(`文件大小不能超过 ${formatFileSize(maxSize)}`);
        return false;
    }

    return true;
}

// 显示错误信息
function showError(message) {
    alert(message);
    resetUploadUI();
}

// 更新文件信息显示
function updateFileInfo(file) {
    fileName.textContent = file.name;
    fileSize.textContent = formatFileSize(file.size);
    fileType.textContent = currentUpgradeType === 'firmware' ? '固件文件' : '资源文件';
    fileInfo.style.display = 'block';

    uploadArea.innerHTML = `
        <i class="fas fa-file-check"></i>
        <p>已选择: ${file.name}</p>
        <p>类型: ${currentUpgradeType === 'firmware' ? '固件升级' : '资源升级'}</p>
    `;
}

// 格式化文件大小
function formatFileSize(bytes) {
    if (bytes === 0) return '0 Bytes';
    const k = 1024;
    const sizes = ['Bytes', 'KB', 'MB', 'GB'];
    const i = Math.floor(Math.log(bytes) / Math.log(k));
    return parseFloat((bytes / Math.pow(k, i)).toFixed(2)) + ' ' + sizes[i];
}

// 处理上传点击
function handleUploadClick() {
    if (!selectedFile) return;

    progressContainer.style.display = 'block';
    uploadBtn.disabled = true;
    fileInfo.style.display = 'none';

    // 开始上传过程
    startUploadProcess();
}

// 开始上传过程
function startUploadProcess() {
    // 这里应该是实际的上传逻辑
    // 目前使用模拟上传
    simulateUpload();
}

// 模拟上传过程
function simulateUpload() {
    let progressValue = 0;
    const interval = setInterval(() => {
        progressValue += Math.random() * 8;
        if (progressValue >= 100) {
            progressValue = 100;
            clearInterval(interval);

            // 上传完成
            handleUploadComplete();
        }

        progress.style.width = `${progressValue}%`;
        progressText.textContent = `上传中: ${Math.round(progressValue)}%`;
    }, 300);
}

// 处理上传完成
function handleUploadComplete() {
    progressText.textContent = '升级完成! 设备将自动重启...';
    progressText.style.color = 'var(--md-sys-color-primary)';

    // 3秒后重置界面
    setTimeout(() => {
        resetUploadUI();
        showSuccessMessage();
    }, 3000);
}

// 显示成功消息
function showSuccessMessage() {
    const message = currentUpgradeType === 'firmware'
        ? '固件升级成功完成！设备已重启。'
        : '资源文件更新成功完成！';

    alert(message);
}

// 重置上传UI
function resetUploadUI() {
    progressContainer.style.display = 'none';
    progress.style.width = '0%';
    progressText.textContent = '准备上传...';
    progressText.style.color = 'var(--md-sys-color-on-surface-variant)';
    uploadBtn.disabled = true;
    selectedFile = null;
    fileInfo.style.display = 'none';

    uploadArea.innerHTML = `
        <i class="fas fa-cloud-upload-alt"></i>
        <p>拖放文件到此处或点击选择文件</p>
    `;
}

// 页面加载完成后初始化
document.addEventListener('DOMContentLoaded', init);