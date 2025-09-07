//
// Created by develop on 2025/9/7.
//

#include "ota_web_ui.h"


// 定义为常量，避免运行时大字符串拼接导致内存溢出
const char OTA_CSS[] = R"CSS(
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }

        body {
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            color: #333;
        }

        .container {
            max-width: 600px;
            margin: 0 auto;
            padding: 20px;
        }

        .header {
            text-align: center;
            color: white;
            margin-bottom: 30px;
        }

        .header h1 {
            font-size: 2.5rem;
            font-weight: 700;
            margin: 0;
            text-shadow: 0 2px 4px rgba(0,0,0,0.3);
        }

        .back-home-btn {
            position: absolute;
            top: 16px;
            right: 16px;
            background: linear-gradient(135deg, #3b82f6, #1d4ed8);
            color: white;
            border: none;
            padding: 12px 20px;
            border-radius: 12px;
            font-size: 1rem;
            font-weight: 600;
            cursor: pointer;
            transition: all 0.3s ease;
            display: inline-flex;
            align-items: center;
            justify-content: center;
            white-space: nowrap;
            z-index: 10;
            box-shadow: 0 2px 8px rgba(59, 130, 246, 0.3);
        }

        .back-home-btn:hover {
            background: linear-gradient(135deg, #2563eb, #1e40af);
            transform: translateY(-2px);
            box-shadow: 0 4px 16px rgba(59, 130, 246, 0.4);
        }

        .subtitle {
            font-size: 1.1rem;
            opacity: 0.9;
        }

        .card {
            position: relative;
            background: white;
            border-radius: 16px;
            padding: 24px;
            padding-top: 70px;
            margin-bottom: 24px;
            box-shadow: 0 8px 32px rgba(0,0,0,0.1);
            backdrop-filter: blur(10px);
        }

        .status-card {
            background: rgba(255,255,255,0.95);
            border-radius: 16px;
            padding: 20px;
            margin-bottom: 24px;
            display: flex;
            align-items: center;
            box-shadow: 0 8px 32px rgba(0,0,0,0.1);
        }

        .status-indicator {
            position: relative;
            width: 48px;
            height: 48px;
            border-radius: 50%;
            margin-right: 16px;
            display: flex;
            align-items: center;
            justify-content: center;
        }

        .status-connected {
            background: #10b981;
        }

        .status-disconnected {
            background: #ef4444;
        }

        .status-ap {
            background: #f59e0b;
        }

        .pulse {
            width: 24px;
            height: 24px;
            background: white;
            border-radius: 50%;
        }

        .current-wifi {
            margin-top: 8px;
            font-weight: 600;
            color: #059669;
            font-size: 0.95rem;
        }

        .current-wifi.hidden {
            display: none;
        }

        .status-connected .pulse {
            animation: pulse-green 2s infinite;
        }

        .status-disconnected .pulse {
            animation: pulse-red 2s infinite;
        }

        .status-ap .pulse {
            animation: pulse-orange 2s infinite;
        }

        @keyframes pulse-green {
            0% { box-shadow: 0 0 0 0 rgba(16, 185, 129, 0.7); }
            70% { box-shadow: 0 0 0 10px rgba(16, 185, 129, 0); }
            100% { box-shadow: 0 0 0 0 rgba(16, 185, 129, 0); }
        }

        @keyframes pulse-red {
            0% { box-shadow: 0 0 0 0 rgba(239, 68, 68, 0.7); }
            70% { box-shadow: 0 0 0 10px rgba(239, 68, 68, 0); }
            100% { box-shadow: 0 0 0 0 rgba(239, 68, 68, 0); }
        }

        @keyframes pulse-orange {
            0% { box-shadow: 0 0 0 0 rgba(245, 158, 11, 0.7); }
            70% { box-shadow: 0 0 0 10px rgba(245, 158, 11, 0); }
            100% { box-shadow: 0 0 0 0 rgba(245, 158, 11, 0); }
        }

        .status-info h3 {
            font-size: 1.25rem;
            margin-bottom: 4px;
        }

        .status-info p {
            color: #666;
            font-size: 0.9rem;
        }

        /* 标签页导航样式 */
        .tab-nav, .ota-tabs {
            display: flex;
            background: rgba(255,255,255,0.95);
            border-radius: 16px;
            padding: 6px;
            margin-bottom: 24px;
            box-shadow: 0 8px 32px rgba(0,0,0,0.1);
        }

        .tab-btn {
            flex: 1;
            background: transparent;
            border: none;
            padding: 12px 20px;
            border-radius: 12px;
            font-size: 1rem;
            font-weight: 500;
            color: #6b7280;
            cursor: pointer;
            display: flex;
            align-items: center;
            justify-content: center;
            transition: all 0.3s ease;
        }

        .tab-btn.active {
            background: linear-gradient(135deg, #3b82f6, #1d4ed8);
            color: white;
            box-shadow: 0 4px 12px rgba(59, 130, 246, 0.4);
        }

        .tab-btn:hover:not(.active) {
            background: #f3f4f6;
            color: #374151;
        }



        /* 标签页内容样式 */
        .tab-content {
            display: none;
        }

        .tab-content.active {
            display: block;
            animation: fadeIn 0.3s ease-in-out;
        }

        @keyframes fadeIn {
            from {
                opacity: 0;
                transform: translateY(10px);
            }
            to {
                opacity: 1;
                transform: translateY(0);
            }
        }

        h2 {
            color: #1f2937;
            margin-bottom: 20px;
            font-size: 1.5rem;
            font-weight: 600;
        }

        .scan-btn {
            width: 100%;
            background: linear-gradient(135deg, #3b82f6, #1d4ed8);
            color: white;
            border: none;
            padding: 12px 20px;
            border-radius: 12px;
            font-size: 1rem;
            font-weight: 500;
            cursor: pointer;
            display: flex;
            align-items: center;
            justify-content: center;
            gap: 8px;
            transition: all 0.3s ease;
            margin-bottom: 20px;
        }

        .scan-btn:hover {
            transform: translateY(-2px);
            box-shadow: 0 8px 25px rgba(59, 130, 246, 0.4);
        }

        .scan-icon {
            font-size: 1.2rem;
        }

        .network-list {
            margin-bottom: 20px;
            max-height: 300px;
            overflow-y: auto;
        }

        /* 已保存WiFi配置样式 */
        .saved-wifi-list {
            margin-bottom: 20px;
        }

        .saved-wifi-item {
            background: #f8fafc;
            border: 2px solid #e2e8f0;
            border-radius: 12px;
            padding: 16px;
            margin-bottom: 12px;
            display: flex;
            justify-content: space-between;
            align-items: center;
            transition: all 0.2s ease;
        }

        .saved-wifi-item:hover {
            background: #f1f5f9;
            border-color: #cbd5e1;
        }

        .saved-wifi-info {
            flex: 1;
            display: flex;
            flex-direction: column;
            gap: 4px;
        }

        .saved-wifi-name {
            font-weight: 600;
            color: #1f2937;
            font-size: 1.2rem;
            line-height: 1.4;
        }

        .saved-wifi-priority {
            color: #6b7280;
            font-size: 0.875rem;
            font-weight: 500;
        }

        .saved-wifi-details {
            color: #6b7280;
            font-size: 0.875rem;
        }

        .saved-wifi-actions {
            display: flex;
            align-items: center;
            gap: 12px;
            flex-wrap: wrap;
        }

        .priority-controls {
            display: flex;
            align-items: center;
            gap: 6px;
            margin-right: 8px;
            background: #f3f4f6;
            padding: 6px 10px;
            border-radius: 8px;
            border: 1px solid #e5e7eb;
        }

        .priority-controls label {
            font-size: 0.875rem;
            color: #6b7280;
            font-weight: 500;
            margin: 0;
        }

        .priority-select {
            padding: 4px 8px;
            border: 1px solid #d1d5db;
            border-radius: 6px;
            font-size: 0.875rem;
            background: white;
            min-width: 50px;
            cursor: pointer;
            transition: all 0.2s ease;
        }

        .priority-select:hover {
            border-color: #3b82f6;
        }

        .priority-select:focus {
            outline: none;
            border-color: #3b82f6;
            box-shadow: 0 0 0 3px rgba(59, 130, 246, 0.1);
        }

        .delete-btn, .connect-btn-small {
            border: none;
            padding: 8px 12px;
            border-radius: 8px;
            font-size: 0.875rem;
            cursor: pointer;
            transition: all 0.2s ease;
            color: white;
        }

        .delete-btn {
            background: #ef4444;
        }

        .delete-btn:hover {
            background: #dc2626;
            transform: translateY(-1px);
        }

        .connect-btn-small {
            background: #10b981;
        }

        .connect-btn-small:hover {
            background: #059669;
            transform: translateY(-1px);
        }

        .connect-btn-small:disabled {
            background: #9ca3af;
            cursor: not-allowed;
            transform: none;
        }

        .priority-badge {
            background: #f59e0b;
            color: white;
            padding: 6px 10px;
            border-radius: 8px;
            font-size: 0.8rem;
            font-weight: 600;
            white-space: nowrap;
            box-shadow: 0 2px 4px rgba(245, 158, 11, 0.2);
        }

        .empty-wifi-message {
            text-align: center;
            color: #6b7280;
            padding: 40px 20px;
            background: #f9fafb;
            border-radius: 12px;
            border: 2px dashed #d1d5db;
        }

        .network-item {
            background: #f8fafc;
            border: 2px solid transparent;
            border-radius: 12px;
            padding: 12px 16px;
            margin-bottom: 8px;
            cursor: pointer;
            display: flex;
            justify-content: space-between;
            align-items: center;
            transition: all 0.2s ease;
        }

        .network-item:hover {
            background: #e2e8f0;
            border-color: #3b82f6;
        }

        .network-item.selected {
            background: #dbeafe;
            border-color: #3b82f6;
        }

        .network-name {
            font-weight: 500;
            color: #1f2937;
        }

        .network-info {
            display: flex;
            align-items: center;
            gap: 8px;
            color: #6b7280;
            font-size: 0.875rem;
        }

        .signal-strength {
            display: flex;
            gap: 2px;
            align-items: flex-end;
            height: 20px;
        }

        .signal-bar {
            width: 3px;
            background: #d1d5db;
            border-radius: 1px;
        }

        .signal-bar.active {
            background: #10b981;
        }

        .form-group {
            margin-bottom: 20px;
            position: relative;
        }

        label {
            display: block;
            margin-bottom: 8px;
            font-weight: 500;
            color: #374151;
        }

        input[type="text"], input[type="password"] {
            width: 100%;
            padding: 12px 16px;
            border: 2px solid #e5e7eb;
            border-radius: 12px;
            font-size: 1rem;
            transition: all 0.3s ease;
            background: #f9fafb;
        }

        input[type="text"]:focus, input[type="password"]:focus {
            outline: none;
            border-color: #3b82f6;
            background: white;
            box-shadow: 0 0 0 3px rgba(59, 130, 246, 0.1);
        }

        .password-toggle {
            position: absolute;
            right: 12px;
            top: 36px;
            cursor: pointer;
            font-size: 1.2rem;
            color: #6b7280;
            user-select: none;
        }

        .connect-btn {
            width: 100%;
            background: linear-gradient(135deg, #10b981, #059669);
            color: white;
            border: none;
            padding: 14px 20px;
            border-radius: 12px;
            font-size: 1.1rem;
            font-weight: 600;
            cursor: pointer;
            position: relative;
            transition: all 0.3s ease;
        }

        .connect-btn:hover:not(:disabled) {
            transform: translateY(-2px);
            box-shadow: 0 8px 25px rgba(16, 185, 129, 0.4);
        }

        .connect-btn:disabled {
            opacity: 0.6;
            cursor: not-allowed;
        }

        .info-grid {
            display: grid;
            gap: 16px;
            margin-bottom: 24px;
        }

        .info-item {
            display: flex;
            justify-content: space-between;
            align-items: center;
            padding: 12px 0;
            border-bottom: 1px solid #e5e7eb;
        }

        .info-item:last-child {
            border-bottom: none;
        }

        .label {
            font-weight: 500;
            color: #374151;
        }

        .value {
            color: #6b7280;
            font-family: 'Courier New', monospace;
            text-align: right !important;
            flex-shrink: 0;
        }

        .info-item .value {
            text-align: right !important;
            display: block;
            width: auto;
        }

        #sramInfo, #psramInfo {
            text-align: right !important;
        }

        .action-buttons {
            display: grid;
            grid-template-columns: 1fr 1fr 1fr;
            gap: 12px;
        }

        .primary-btn, .danger-btn, .warning-btn, .secondary-btn, .success-btn {
            padding: 12px 16px;
            border: none;
            border-radius: 12px;
            font-weight: 500;
            cursor: pointer;
            transition: all 0.3s ease;
            display: flex;
            align-items: center;
            justify-content: center;
            font-size: 0.9rem;
        }

        .primary-btn {
            background: linear-gradient(135deg, #3b82f6, #1d4ed8);
            color: white;
        }

        .primary-btn:hover {
            transform: translateY(-2px);
            box-shadow: 0 8px 25px rgba(59, 130, 246, 0.4);
        }

        .danger-btn {
            background: linear-gradient(135deg, #ef4444, #dc2626);
            color: white;
        }

        .danger-btn:hover {
            transform: translateY(-2px);
            box-shadow: 0 8px 25px rgba(239, 68, 68, 0.4);
        }

        .warning-btn {
            background: linear-gradient(135deg, #f59e0b, #d97706);
            color: white;
        }

        .warning-btn:hover {
            transform: translateY(-2px);
            box-shadow: 0 8px 25px rgba(245, 158, 11, 0.4);
        }

        .secondary-btn {
            background: #f3f4f6;
            color: #374151;
        }

        .secondary-btn:hover {
            background: #e5e7eb;
            transform: translateY(-2px);
        }

        .success-btn {
            background: linear-gradient(135deg, #10b981, #059669);
            color: white;
        }

        .success-btn:hover {
            transform: translateY(-2px);
            box-shadow: 0 8px 25px rgba(16, 185, 129, 0.4);
        }

        /* 设备管理按钮 - 统一基础样式和字体大小 */
        .refresh-btn, .settings-btn, .weather-btn, .files-btn, .ota-btn, .reboot-btn, .reset-btn {
            padding: 12px 16px;
            border: none;
            border-radius: 12px;
            font-weight: 500;
            font-size: 1rem;
            cursor: pointer;
            transition: all 0.3s ease;
            display: flex;
            align-items: center;
            justify-content: center;
            color: white;
        }

        .refresh-btn, .settings-btn, .weather-btn, .files-btn, .ota-btn, .reboot-btn, .reset-btn {
            transform: translateY(0);
        }

        .refresh-btn:hover, .settings-btn:hover, .weather-btn:hover, .files-btn:hover,
        .ota-btn:hover, .reboot-btn:hover, .reset-btn:hover {
            transform: translateY(-2px);
        }

        .refresh-btn {
            background: linear-gradient(135deg, #06b6d4, #0891b2);
        }

        .refresh-btn:hover {
            box-shadow: 0 8px 25px rgba(6, 182, 212, 0.4);
        }

        .settings-btn {
            background: linear-gradient(135deg, #3b82f6, #1d4ed8);
        }

        .settings-btn:hover {
            box-shadow: 0 8px 25px rgba(59, 130, 246, 0.4);
        }

        .weather-btn {
            background: linear-gradient(135deg, #14b8a6, #0d9488);
        }

        .weather-btn:hover {
            box-shadow: 0 8px 25px rgba(20, 184, 166, 0.4);
        }

        .files-btn {
            background: linear-gradient(135deg, #10b981, #059669);
        }

        .files-btn:hover {
            box-shadow: 0 8px 25px rgba(16, 185, 129, 0.4);
        }

        .ota-btn {
            background: linear-gradient(135deg, #8b5cf6, #7c3aed);
        }

        .ota-btn:hover {
            box-shadow: 0 8px 25px rgba(139, 92, 246, 0.4);
        }

        .reboot-btn {
            background: linear-gradient(135deg, #f59e0b, #d97706);
        }

        .reboot-btn:hover {
            box-shadow: 0 8px 25px rgba(245, 158, 11, 0.4);
        }

        .reset-btn {
            background: linear-gradient(135deg, #ef4444, #dc2626);
        }

        .reset-btn:hover {
            box-shadow: 0 8px 25px rgba(239, 68, 68, 0.4);
        }


        .toast {
            position: fixed;
            top: 20px;
            right: 20px;
            background: white;
            border-radius: 12px;
            padding: 16px 20px;
            box-shadow: 0 8px 32px rgba(0,0,0,0.15);
            transform: translateX(400px);
            transition: transform 0.3s ease;
            z-index: 1000;
        }

        .toast.show {
            transform: translateX(0);
        }

        .toast.success {
            border-left: 4px solid #10b981;
        }

        .toast.error {
            border-left: 4px solid #ef4444;
        }

        .loading {
            display: flex;
            align-items: center;
            justify-content: center;
            gap: 12px;
            padding: 20px;
            color: #6b7280;
        }

        .spinner, .spinner-sm {
            border: 2px solid #e5e7eb;
            border-top: 2px solid #3b82f6;
            border-radius: 50%;
            animation: spin 1s linear infinite;
        }

        .spinner {
            width: 20px;
            height: 20px;
        }

        .spinner-sm {
            width: 16px;
            height: 16px;
        }

        @keyframes spin {
            0% { transform: rotate(0deg); }
            100% { transform: rotate(360deg); }
        }

        .hidden {
            display: none !important;
        }

        .btn-loading {
            display: flex;
            align-items: center;
            justify-content: center;
            gap: 8px;
        }

        .btn-loading .spinner-sm {
            border-color: rgba(255,255,255,0.3);
            border-top-color: white;
        }

        .btn-loading.absolute {
            position: absolute;
            top: 50%;
            left: 50%;
            transform: translate(-50%, -50%);
        }

        /* OTA升级相关样式 */
        .ota-section {
            text-align: center;
        }

        .ota-info {
            margin-bottom: 24px;
            padding: 20px;
            background: #f0f9ff;
            border-radius: 12px;
            border: 2px solid #0ea5e9;
        }

        .info-text {
            color: #0c4a6e;
            font-size: 1.1rem;
            font-weight: 500;
            margin-bottom: 8px;
        }

        .warning-text {
            color: #dc2626;
            font-size: 0.95rem;
            font-weight: 500;
        }

        .file-upload-section {
            margin-bottom: 24px;
        }

        .file-select-btn {
            background: linear-gradient(135deg, #6366f1, #4f46e5);
            color: white;
            border: none;
            padding: 14px 24px;
            border-radius: 12px;
            font-size: 1.1rem;
            font-weight: 600;
            cursor: pointer;
            transition: all 0.3s ease;
            margin-bottom: 16px;
        }

        .file-select-btn:hover {
            transform: translateY(-2px);
            box-shadow: 0 8px 25px rgba(99, 102, 241, 0.4);
        }

        .file-info {
            background: #f8fafc;
            border: 2px solid #e2e8f0;
            border-radius: 12px;
            padding: 16px;
            text-align: left;
        }

        .file-info p {
            margin-bottom: 8px;
            color: #374151;
        }

        .file-info p:last-child {
            margin-bottom: 0;
        }

        .upload-section {
            margin-bottom: 24px;
        }

        .upload-btn {
            background: linear-gradient(135deg, #10b981, #059669);
            color: white;
            border: none;
            padding: 16px 32px;
            border-radius: 12px;
            font-size: 1.2rem;
            font-weight: 600;
            cursor: pointer;
            position: relative;
            transition: all 0.3s ease;
            min-width: 200px;
        }

        .upload-btn:hover:not(:disabled) {
            transform: translateY(-2px);
            box-shadow: 0 8px 25px rgba(16, 185, 129, 0.4);
        }

        .upload-btn:disabled {
            background: #9ca3af;
            cursor: not-allowed;
            transform: none;
        }

        .ota-progress {
            background: #f8fafc;
            border: 2px solid #e2e8f0;
            border-radius: 12px;
            padding: 24px;
            margin-bottom: 24px;
        }

        .progress-info {
            margin-bottom: 20px;
        }

        .progress-info h3 {
            color: #1f2937;
            margin-bottom: 8px;
            font-size: 1.25rem;
        }

        .progress-info p {
            color: #6b7280;
            font-size: 0.95rem;
        }

        .progress-bar {
            width: 100%;
            height: 12px;
            background: #e5e7eb;
            border-radius: 6px;
            overflow: hidden;
            margin-bottom: 12px;
        }

        .progress-fill {
            height: 100%;
            background: linear-gradient(135deg, #10b981, #059669);
            border-radius: 6px;
            transition: width 0.3s ease;
            width: 0%;
        }

        .progress-text {
            display: flex;
            justify-content: space-between;
            align-items: center;
            font-size: 0.875rem;
            color: #6b7280;
        }

        .ota-result {
            padding: 24px;
            border-radius: 12px;
            text-align: center;
        }

        .result-success {
            background: #f0fdf4;
            border: 2px solid #22c55e;
            border-radius: 12px;
        }

        .result-success h3 {
            color: #166534;
            margin-bottom: 12px;
        }

        .result-success p {
            color: #15803d;
            margin-bottom: 20px;
        }

        .result-error {
            background: #fef2f2;
            border: 2px solid #ef4444;
            border-radius: 12px;
        }

        .result-error h3 {
            color: #991b1b;
            margin-bottom: 12px;
        }

        .result-error p {
            color: #dc2626;
            margin-bottom: 20px;
        }

        .reboot-btn, .retry-btn {
            background: linear-gradient(135deg, #f59e0b, #d97706);
            color: white;
            border: none;
            padding: 12px 24px;
            border-radius: 12px;
            font-size: 1rem;
            font-weight: 600;
            cursor: pointer;
            transition: all 0.3s ease;
        }

        .reboot-btn:hover, .retry-btn:hover {
            transform: translateY(-2px);
            box-shadow: 0 8px 25px rgba(245, 158, 11, 0.4);
        }

        .retry-btn {
            background: linear-gradient(135deg, #3b82f6, #1d4ed8);
        }

        .retry-btn:hover {
            box-shadow: 0 8px 25px rgba(59, 130, 246, 0.4);
        }

        /* 屏幕配置相关样式 */
        .screen-config-section {
            padding: 20px 0;
        }

        .brightness-control {
            margin-bottom: 24px;
        }

        .slider-container {
            margin-bottom: 20px;
        }

        .slider-label {
            display: block;
            margin-bottom: 12px;
            font-weight: 600;
            color: #374151;
            font-size: 1.1rem;
        }

        .slider-wrapper {
            position: relative;
            margin-bottom: 12px;
        }

        .brightness-slider {
            width: 100%;
            height: 8px;
            border-radius: 8px;
            background: linear-gradient(to right, #f3f4f6, #3b82f6);
            outline: none;
            -webkit-appearance: none;
            cursor: pointer;
        }

        .brightness-slider::-webkit-slider-thumb {
            -webkit-appearance: none;
            appearance: none;
            width: 24px;
            height: 24px;
            border-radius: 50%;
            background: linear-gradient(135deg, #3b82f6, #1d4ed8);
            box-shadow: 0 4px 12px rgba(59, 130, 246, 0.4);
            cursor: pointer;
            transition: all 0.3s ease;
        }

        .brightness-slider::-webkit-slider-thumb:hover {
            transform: scale(1.1);
            box-shadow: 0 6px 16px rgba(59, 130, 246, 0.6);
        }

        .brightness-slider::-moz-range-thumb {
            width: 24px;
            height: 24px;
            border-radius: 50%;
            background: linear-gradient(135deg, #3b82f6, #1d4ed8);
            box-shadow: 0 4px 12px rgba(59, 130, 246, 0.4);
            cursor: pointer;
            border: none;
            transition: all 0.3s ease;
        }

        .brightness-display {
            text-align: center;
            font-size: 1.2rem;
            font-weight: 600;
            color: #1f2937;
            background: #f8fafc;
            padding: 12px;
            border-radius: 12px;
            border: 2px solid #e2e8f0;
        }

        .brightness-presets {
            margin-top: 24px;
        }

        .brightness-presets h3 {
            margin-bottom: 16px;
            color: #374151;
            font-size: 1.1rem;
            font-weight: 600;
        }

        .preset-buttons {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(100px, 1fr));
            gap: 12px;
        }

        .preset-btn {
            padding: 12px 16px;
            border: none;
            border-radius: 12px;
            background: linear-gradient(135deg, #6b7280, #4b5563);
            color: white;
            font-weight: 500;
            cursor: pointer;
            transition: all 0.3s ease;
            font-size: 0.9rem;
        }

        .preset-btn:hover {
            transform: translateY(-2px);
            box-shadow: 0 8px 25px rgba(107, 114, 128, 0.4);
        }

        .preset-btn.active {
            background: linear-gradient(135deg, #3b82f6, #1d4ed8);
            box-shadow: 0 4px 12px rgba(59, 130, 246, 0.4);
        }

        .screen-settings {
            margin-bottom: 24px;
        }

        .setting-item {
            display: flex;
            justify-content: space-between;
            align-items: center;
            padding: 16px 0;
            border-bottom: 1px solid #e5e7eb;
        }

        .setting-item:last-child {
            border-bottom: none;
        }

        .setting-label {
            font-weight: 500;
            color: #374151;
            font-size: 1rem;
        }

        @media (max-width: 640px) {
            .container {
                padding: 16px;
            }

            .header h1 {
                font-size: 2rem;
            }

            .card {
                padding: 20px;
            }

            .action-buttons {
                flex-direction: column;
            }

            .container {
                padding: 16px;
            }

            .card {
                padding: 20px;
                padding-top: 60px;
            }

            .back-home-btn {
                top: 12px;
                right: 12px;
                font-size: 0.9rem;
                padding: 10px 16px;
            }

            .header h1 {
                font-size: 2rem;
            }
        }
        )CSS";

const char OTA_PAGE_CSS[] = R"CSS(
        /* OTA页面专用样式 */
        .ota-section {
            display: flex;
            flex-direction: column;
            gap: 24px;
        }

        /* OTA选项卡样式 */
        .ota-tabs {
            display: flex;
            background: #f8fafc;
            border-radius: 12px;
            padding: 4px;
            margin-bottom: 24px;
            border: 1px solid #e2e8f0;
        }

        .tab-btn {
            flex: 1;
            background: none;
            border: none;
            padding: 12px 16px;
            border-radius: 8px;
            font-size: 1rem;
            font-weight: 600;
            color: #64748b;
            cursor: pointer;
            transition: all 0.3s ease;
            position: relative;
        }

        .tab-btn.active {
            background: white;
            color: #3b82f6;
            box-shadow: 0 2px 8px rgba(59, 130, 246, 0.2);
        }

        .tab-btn:hover:not(.active) {
            color: #3b82f6;
            background: rgba(59, 130, 246, 0.05);
        }

        /* 选项卡内容样式 */
        .tab-content {
            display: none;
            animation: fadeIn 0.3s ease-in-out;
        }

        .tab-content.active {
            display: block;
        }

        @keyframes fadeIn {
            from { opacity: 0; transform: translateY(10px); }
            to { opacity: 1; transform: translateY(0); }
        }

        /* 服务器OTA样式 */
        .server-ota-section {
            display: flex;
            flex-direction: column;
            gap: 24px;
        }

        .server-info {
            text-align: center;
            padding: 24px;
            background: linear-gradient(135deg, #e0f2fe, #b3e5fc);
            border-radius: 16px;
            border: 1px solid #0ea5e9;
        }

        .server-text {
            font-size: 1.2rem;
            font-weight: 600;
            color: #0c4a6e;
            margin-bottom: 8px;
        }

        .server-url {
            color: #0369a1;
            font-weight: 500;
            font-family: monospace;
            background: rgba(255, 255, 255, 0.7);
            padding: 8px 16px;
            border-radius: 8px;
            display: inline-block;
            margin: 0;
        }

        .server-actions {
            display: flex;
            gap: 16px;
            justify-content: center;
            flex-wrap: wrap;
        }

        .server-btn {
            background: linear-gradient(135deg, #06b6d4, #0891b2);
            color: white;
            border: none;
            padding: 14px 28px;
            border-radius: 12px;
            font-size: 1rem;
            font-weight: 600;
            cursor: pointer;
            transition: all 0.3s ease;
            box-shadow: 0 4px 16px rgba(6, 182, 212, 0.3);
            position: relative;
            overflow: hidden;
        }

        .server-btn:hover {
            transform: translateY(-2px);
            box-shadow: 0 6px 20px rgba(6, 182, 212, 0.4);
        }

        .server-btn:active {
            transform: translateY(0);
        }

        .server-btn.version-btn {
            background: linear-gradient(135deg, #8b5cf6, #7c3aed);
            box-shadow: 0 4px 16px rgba(139, 92, 246, 0.3);
        }

        .server-btn.version-btn:hover {
            box-shadow: 0 6px 20px rgba(139, 92, 246, 0.4);
        }

        .server-btn.ota-btn {
            background: linear-gradient(135deg, #10b981, #059669);
            box-shadow: 0 4px 16px rgba(16, 185, 129, 0.3);
        }

        .server-btn.ota-btn:hover {
            box-shadow: 0 6px 20px rgba(16, 185, 129, 0.4);
        }

        .version-info {
            background: white;
            border-radius: 12px;
            padding: 20px;
            border: 1px solid #e2e8f0;
            box-shadow: 0 2px 8px rgba(0, 0, 0, 0.05);
        }

        .version-info h4 {
            color: #1f2937;
            margin-bottom: 16px;
            font-size: 1.1rem;
            font-weight: 600;
        }

        .version-info p {
            margin-bottom: 8px;
            color: #4b5563;
            font-size: 0.95rem;
        }

        .version-info p:last-child {
            margin-bottom: 0;
        }

        .version-info strong {
            color: #1f2937;
            font-weight: 600;
        }

        .ota-info {
            text-align: center;
            padding: 24px;
            background: linear-gradient(135deg, #f8fafc, #e2e8f0);
            border-radius: 16px;
            border: 1px solid #cbd5e1;
            position: relative;
            overflow: hidden;
        }

        .ota-info::before {
            content: '';
            position: absolute;
            top: 0;
            left: -100%;
            width: 100%;
            height: 100%;
            background: linear-gradient(90deg, transparent, rgba(59, 130, 246, 0.1), transparent);
            animation: infoShine 3s infinite;
        }

        @keyframes infoShine {
            0% { left: -100%; }
            100% { left: 100%; }
        }

        .info-text {
            font-size: 1.2rem;
            font-weight: 600;
            color: #1f2937;
            margin-bottom: 12px;
            display: flex;
            align-items: center;
            justify-content: center;
            gap: 8px;
        }

        .warning-text {
            color: #dc2626;
            font-weight: 500;
            font-size: 0.95rem;
            display: flex;
            align-items: center;
            justify-content: center;
            gap: 8px;
            padding: 8px 16px;
            background: rgba(220, 38, 38, 0.1);
            border-radius: 8px;
            border: 1px solid rgba(220, 38, 38, 0.2);
            margin: 0 auto;
            max-width: fit-content;
        }

        /* 文件上传区域 */
        .file-upload-section {
            display: flex;
            flex-direction: column;
            align-items: center;
            gap: 20px;
            padding: 32px;
            background: white;
            border-radius: 20px;
            border: 2px dashed #cbd5e1;
            transition: all 0.3s ease;
            position: relative;
            overflow: hidden;
        }

        .file-upload-section:hover {
            border-color: #3b82f6;
            background: #f8fafc;
            transform: translateY(-2px);
            box-shadow: 0 8px 30px rgba(59, 130, 246, 0.1);
        }

        .file-select-btn {
            background: linear-gradient(135deg, #3b82f6, #1d4ed8);
            color: white;
            border: none;
            padding: 16px 32px;
            border-radius: 16px;
            font-size: 1.1rem;
            font-weight: 600;
            cursor: pointer;
            transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
            display: flex;
            align-items: center;
            justify-content: center;
            gap: 10px;
            box-shadow: 0 4px 20px rgba(59, 130, 246, 0.3);
            position: relative;
            overflow: hidden;
        }

        .file-select-btn::after {
            content: '';
            position: absolute;
            top: 0;
            left: -100%;
            width: 100%;
            height: 100%;
            background: linear-gradient(90deg, transparent, rgba(255,255,255,0.2), transparent);
            transition: left 0.5s;
        }

        .file-select-btn:hover::after {
            left: 100%;
        }

        .file-select-btn:hover {
            transform: translateY(-3px) scale(1.02);
            box-shadow: 0 8px 30px rgba(59, 130, 246, 0.4);
        }

        .file-select-btn:active {
            transform: translateY(-1px) scale(0.98);
        }

        /* 文件信息显示 */
        .file-info {
            background: linear-gradient(135deg, #f0f9ff, #e0f2fe);
            border: 1px solid #0ea5e9;
            border-radius: 12px;
            padding: 20px;
            width: 100%;
            max-width: 400px;
            animation: slideInDown 0.5s ease-out;
        }

        @keyframes slideInDown {
            from {
                opacity: 0;
                transform: translateY(-20px);
            }
            to {
                opacity: 1;
                transform: translateY(0);
            }
        }

        .file-info p {
            margin-bottom: 8px;
            font-size: 0.95rem;
            color: #0f172a;
        }

        .file-info p:last-child {
            margin-bottom: 0;
        }

        .file-info strong {
            color: #0369a1;
            font-weight: 600;
        }

        /* 上传按钮区域 */
        .upload-section {
            display: flex;
            justify-content: center;
            align-items: center;
        }

        .upload-btn {
            background: linear-gradient(135deg, #10b981, #059669);
            color: white;
            border: none;
            padding: 18px 40px;
            border-radius: 16px;
            font-size: 1.2rem;
            font-weight: 700;
            cursor: pointer;
            transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
            display: flex;
            align-items: center;
            justify-content: center;
            gap: 12px;
            box-shadow: 0 6px 25px rgba(16, 185, 129, 0.3);
            position: relative;
            overflow: hidden;
            min-width: 200px;
            min-height: 56px;
        }

        .upload-btn::before {
            content: '';
            position: absolute;
            top: 0;
            left: -100%;
            width: 100%;
            height: 100%;
            background: linear-gradient(90deg, transparent, rgba(255,255,255,0.2), transparent);
            transition: left 0.5s;
        }

        .upload-btn:hover:not(:disabled)::before {
            left: 100%;
        }

        .upload-btn:hover:not(:disabled) {
            transform: translateY(-3px) scale(1.05);
            box-shadow: 0 10px 40px rgba(16, 185, 129, 0.4);
        }

        .upload-btn:disabled {
            background: linear-gradient(135deg, #9ca3af, #6b7280);
            cursor: not-allowed;
            transform: none;
            box-shadow: 0 2px 8px rgba(156, 163, 175, 0.2);
        }

        .btn-loading {
            display: flex;
            align-items: center;
            gap: 8px;
        }

        /* 进度显示区域 */
        .ota-progress {
            background: white;
            border-radius: 20px;
            padding: 32px;
            border: 1px solid #e2e8f0;
            box-shadow: 0 4px 20px rgba(0,0,0,0.08);
            animation: fadeInUp 0.5s ease-out;
        }

        @keyframes fadeInUp {
            from {
                opacity: 0;
                transform: translateY(20px);
            }
            to {
                opacity: 1;
                transform: translateY(0);
            }
        }

        .progress-info {
            text-align: center;
            margin-bottom: 24px;
        }

        .progress-info h3 {
            font-size: 1.4rem;
            font-weight: 700;
            color: #1f2937;
            margin-bottom: 8px;
            display: flex;
            align-items: center;
            justify-content: center;
            gap: 10px;
        }

        @keyframes rotate {
            from { transform: rotate(0deg); }
            to { transform: rotate(360deg); }
        }

        .progress-info p {
            color: #6b7280;
            font-size: 1rem;
        }

        /* 进度条美化 */
        .progress-bar {
            width: 100%;
            height: 12px;
            background: #f1f5f9;
            border-radius: 6px;
            overflow: hidden;
            margin-bottom: 16px;
            position: relative;
            box-shadow: inset 0 2px 4px rgba(0,0,0,0.1);
        }

        .progress-fill {
            height: 100%;
            background: linear-gradient(90deg, #10b981, #059669, #34d399);
            border-radius: 6px;
            width: 0%;
            transition: width 0.3s ease;
            position: relative;
            overflow: hidden;
        }

        .progress-fill::after {
            content: '';
            position: absolute;
            top: 0;
            left: 0;
            bottom: 0;
            right: 0;
            background: linear-gradient(
                90deg,
                transparent,
                rgba(255,255,255,0.4),
                transparent
            );
            animation: progressShine 2s infinite;
        }

        @keyframes progressShine {
            0% { transform: translateX(-100%); }
            100% { transform: translateX(100%); }
        }

        .progress-text {
            display: flex;
            justify-content: space-between;
            align-items: center;
            font-size: 0.9rem;
            color: #4b5563;
            font-weight: 500;
        }

        /* 结果显示区域 */
        .ota-result {
            background: white;
            border-radius: 20px;
            padding: 40px;
            text-align: center;
            box-shadow: 0 8px 30px rgba(0,0,0,0.1);
            animation: bounceIn 0.6s ease-out;
        }

        @keyframes bounceIn {
            0% {
                opacity: 0;
                transform: scale(0.3);
            }
            50% {
                opacity: 1;
                transform: scale(1.05);
            }
            70% {
                transform: scale(0.9);
            }
            100% {
                opacity: 1;
                transform: scale(1);
            }
        }

        .result-success {
            color: #059669;
        }

        .result-success h3 {
            font-size: 1.8rem;
            font-weight: 700;
            margin-bottom: 16px;
            display: flex;
            align-items: center;
            justify-content: center;
            gap: 12px;
        }

        .result-success p {
            font-size: 1.1rem;
            line-height: 1.6;
        }

        @keyframes pulse {
            0%, 100% { transform: scale(1); }
            50% { transform: scale(1.1); }
        }

        .result-error {
            color: #dc2626;
        }

        .result-error h3 {
            font-size: 1.6rem;
            font-weight: 700;
            margin-bottom: 16px;
            display: flex;
            align-items: center;
            justify-content: center;
            gap: 12px;
        }

        .result-error p {
            font-size: 1rem;
            margin-bottom: 20px;
            line-height: 1.6;
        }

        .retry-btn {
            background: linear-gradient(135deg, #f59e0b, #d97706);
            color: white;
            border: none;
            padding: 12px 24px;
            border-radius: 12px;
            font-size: 1rem;
            font-weight: 600;
            cursor: pointer;
            transition: all 0.3s ease;
        }

        .retry-btn:hover {
            transform: translateY(-2px);
            box-shadow: 0 8px 25px rgba(245, 158, 11, 0.4);
        }

        /* 加载动画 */
        .spinner-sm {
            width: 20px;
            height: 20px;
            border: 2px solid rgba(255,255,255,0.3);
            border-radius: 50%;
            border-top-color: white;
            animation: spin 1s ease-in-out infinite;
        }

        @keyframes spin {
            to { transform: rotate(360deg); }
        }

        /* 隐藏元素 */
        .hidden {
            display: none !important;
        }

        /* 响应式设计 */
        @media (max-width: 768px) {
            .file-upload-section {
                padding: 24px 16px;
            }

            .file-select-btn {
                padding: 14px 24px;
                font-size: 1rem;
            }

            .upload-btn {
                padding: 16px 32px;
                font-size: 1.1rem;
                min-width: 180px;
            }

            .ota-progress, .ota-result {
                padding: 24px 20px;
            }

            .result-success h3, .result-error h3 {
                font-size: 1.4rem;
            }
        }

        @media (max-width: 480px) {
            .container {
                padding: 16px;
            }

            .card {
                padding: 20px;
                padding-top: 60px;
            }

            .file-upload-section {
                padding: 20px 12px;
            }

            .upload-btn {
                min-width: 160px;
                padding: 14px 24px;
            }
        }
    )CSS";

const char OTA_JS[] = R"JS(
// 基础工具函数
function showToast(message, type) {
    const toast = document.getElementById('toast');
    const toastMessage = document.getElementById('toastMessage');
    toastMessage.textContent = message;
    toast.className = 'toast show ' + (type || 'info');
    setTimeout(() => { toast.className = 'toast hidden'; }, 3000);
}

function formatBytes(bytes) {
    if (bytes === 0) return '0 字节';
    const k = 1024;
    const sizes = ['字节', 'KB', 'MB', 'GB'];
    const i = Math.floor(Math.log(bytes) / Math.log(k));
    return parseFloat((bytes / Math.pow(k, i)).toFixed(2)) + ' ' + sizes[i];
}

let otaStatusInterval;
let selectedFile = null;

document.addEventListener('DOMContentLoaded', function() {
    initOTAHandlers();
    initTabSwitching();
});

function initTabSwitching() {
    const tabButtons = document.querySelectorAll('.tab-btn');
    for (let i = 0; i < tabButtons.length; i++) {
        tabButtons[i].addEventListener('click', function() {
            const tabName = this.getAttribute('data-tab');
            switchOTATab(tabName);
        });
    }
}

function switchOTATab(tabName) {
    // 移除所有标签页按钮的active类
    const tabButtons = document.querySelectorAll('.tab-btn');
    for (let i = 0; i < tabButtons.length; i++) {
        tabButtons[i].classList.remove('active');
    }

    // 移除所有标签页内容的active类
    const tabContents = document.querySelectorAll('.tab-content');
    for (let i = 0; i < tabContents.length; i++) {
        tabContents[i].classList.remove('active');
    }

    // 激活选中的标签页按钮
    const activeButton = document.querySelector('[data-tab=\"' + tabName + '\"]');
    if (activeButton) {
        activeButton.classList.add('active');
    }

    // 激活对应的标签页内容
    const activeContent = document.getElementById(tabName + 'Tab');
    if (activeContent) {
        activeContent.classList.add('active');
    }
}

function initOTAHandlers() {
    const selectFileBtn = document.getElementById('selectFileBtn');
    const firmwareFile = document.getElementById('firmwareFile');
    const uploadBtn = document.getElementById('uploadBtn');
    const retryBtn = document.getElementById('retryBtn');
    const checkVersionBtn = document.getElementById('checkVersionBtn');
    const serverOTABtn = document.getElementById('serverOTABtn');

    if (selectFileBtn) selectFileBtn.addEventListener('click', () => firmwareFile.click());
    if (firmwareFile) firmwareFile.addEventListener('change', handleFileSelect);
    if (uploadBtn) uploadBtn.addEventListener('click', startOTAUpload);
    if (retryBtn) retryBtn.addEventListener('click', resetOTAUI);
    if (checkVersionBtn) checkVersionBtn.addEventListener('click', checkServerVersion);
    if (serverOTABtn) serverOTABtn.addEventListener('click', startServerOTA);
}

function handleFileSelect(event) {
    const file = event.target.files[0];
    if (file) {
        if (!file.name.endsWith('.bin')) {
            showToast('请选择.bin格式的固件文件', 'error');
            event.target.value = '';
            return;
        }
        selectedFile = file;
        document.getElementById('fileName').textContent = file.name;
        document.getElementById('fileSize').textContent = formatBytes(file.size);
        document.getElementById('fileInfo').classList.remove('hidden');
        document.getElementById('uploadBtn').disabled = false;
    }
}

async function startOTAUpload() {
    if (!selectedFile) { showToast('请先选择固件文件', 'error'); return; }
    const uploadBtn = document.getElementById('uploadBtn');
    uploadBtn.disabled = true;
    // 隐藏按钮文字，显示loading状态
    uploadBtn.querySelector('.btn-text').style.display = 'none';
    const btnLoading = uploadBtn.querySelector('.btn-loading');
    btnLoading.classList.remove('hidden');
    btnLoading.innerHTML = '<div class="spinner-sm"></div><span>升级中...</span>';
    document.getElementById('otaProgress').classList.remove('hidden');
    // 初始化进度显示
    document.getElementById('progressSize').textContent = '0 / ' + formatBytes(selectedFile.size);
    try {
        const formData = new FormData();
        formData.append('firmware', selectedFile);
        const xhr = new XMLHttpRequest();
        xhr.upload.addEventListener('progress', (e) => {
            if (e.lengthComputable) {
                const percent = Math.round((e.loaded / e.total) * 100);
                document.getElementById('progressFill').style.width = percent + '%';
                document.getElementById('progressPercent').textContent = percent + '%';
                document.getElementById('progressSize').textContent = formatBytes(e.loaded) + ' / ' + formatBytes(e.total);
            }
        });
        otaStatusInterval = setInterval(updateOTAStatus, 300);
        xhr.open('POST', '/ota-upload');
        xhr.send(formData);
    } catch (error) {
        showOTAResult(false, 'OTA升级失败: ' + error.message);
    }
}

async function updateOTAStatus() {
    try {
        const response = await fetch('/ota-status');
        const status = await response.json();
        document.getElementById('otaStatusTitle').textContent = status.message || '升级中...';
        const progress = status.progress || 0;
        document.getElementById('progressFill').style.width = progress + '%';
        document.getElementById('progressPercent').textContent = progress.toFixed(1) + '%';
        // 更新大小信息
        if (status.totalSize && status.writtenSize !== undefined) {
            document.getElementById('progressSize').textContent = formatBytes(status.writtenSize) + ' / ' + formatBytes(status.totalSize);
        }
        if (status.status === 'failed') {
            clearInterval(otaStatusInterval);
            showOTAResult(false, status.error || '升级失败');
        } else if (status.status === 'success') {
            clearInterval(otaStatusInterval);
            showOTAResult(true, '固件升级成功！');
        }
    } catch (error) { console.error('获取OTA状态失败:', error); }
}

function showOTAResult(success, message) {
    document.getElementById('otaProgress').classList.add('hidden');
    document.getElementById('otaResult').classList.remove('hidden');
    if (success) {
        document.getElementById('successResult').classList.remove('hidden');
        setTimeout(() => {
            fetch('/ota-reboot', {method: 'POST'}).then(() => {
                // 等待设备重启并检测重连
                waitForDeviceReboot();
            }).catch(() => {
                // 重启请求失败也要等待重连
                waitForDeviceReboot();
            });
        }, 3000);
    } else {
        document.getElementById('errorResult').classList.remove('hidden');
        document.getElementById('errorMessage').textContent = message;
    }
}

function resetOTAUI() {
    selectedFile = null;
    document.getElementById('firmwareFile').value = '';
    document.getElementById('fileInfo').classList.add('hidden');
    const uploadBtn = document.getElementById('uploadBtn');
    uploadBtn.disabled = true;
    // 恢复按钮原始状态
    uploadBtn.querySelector('.btn-text').style.display = '';
    uploadBtn.querySelector('.btn-text').textContent = '选择文件开始升级';
    uploadBtn.querySelector('.btn-loading').classList.add('hidden');
    document.getElementById('otaProgress').classList.add('hidden');
    document.getElementById('otaResult').classList.add('hidden');
    document.getElementById('successResult').classList.add('hidden');
    document.getElementById('errorResult').classList.add('hidden');
    if (otaStatusInterval) clearInterval(otaStatusInterval);
}

function waitForDeviceReboot() {
    // 更新成功页面显示
    const successResult = document.getElementById('successResult');
    if (successResult) {
        successResult.innerHTML = '<h3>设备重启中...</h3><p>请稍候，设备重启完成后将自动跳转到首页。</p>';
    }

    let retryCount = 0;
    const maxRetries = 30; // 最大等待30次，每次2秒

    function checkDeviceStatus() {
        retryCount++;

        fetch('/status', { method: 'GET', timeout: 5000 })
            .then(response => {
                if (response.ok) {
                    // 设备恢复正常，跳转到首页
                    showToast('升级成功，正在跳转到首页...', 'success');
                    setTimeout(() => {
                        window.location.href = '/';
                    }, 1000);
                } else {
                    // 继续等待
                    scheduleNextCheck();
                }
            })
            .catch(() => {
                // 设备还未恢复，继续等待
                scheduleNextCheck();
            });
    }

    function scheduleNextCheck() {
        if (retryCount < maxRetries) {
            setTimeout(checkDeviceStatus, 2000); // 每2秒检查一次
        } else {
            // 超时，手动跳转
            if (successResult) {
                successResult.innerHTML = '<h3>升级完成</h3><p>设备重启时间较长，请手动刷新页面或<a href=\"/\">点击这里返回首页</a>。</p>';
            }
        }
    }

    // 等待5秒后开始检测（给设备重启留时间）
    setTimeout(checkDeviceStatus, 5000);
}

/* 服务器OTA相关函数 */
async function checkServerVersion() {
    const checkVersionBtn = document.getElementById('checkVersionBtn');
    const versionInfo = document.getElementById('versionInfo');
    const currentVersion = document.getElementById('currentVersion');
    const serverVersion = document.getElementById('serverVersion');

    checkVersionBtn.disabled = true;
    checkVersionBtn.textContent = '检查中...';

    try {
        const response = await fetch('/api/ota/firmware-version');
        const data = await response.json();

        if (data.success) {
            versionInfo.classList.remove('hidden');
            currentVersion.textContent = data.currentVersion || '1.0.0';

            const serverOTABtn = document.getElementById('serverOTABtn');

            if (data.serverVersion && data.serverVersion.version) {
                serverVersion.textContent = data.serverVersion.version;

                // 根据版本比较结果设置UI状态
                if (data.versionStatus === 'up_to_date') {
                    // 版本相同，已是最新
                    serverVersion.style.color = '#10b981';
                    serverOTABtn.disabled = true;
                    serverOTABtn.textContent = '已是最新版本';
                    serverOTABtn.style.backgroundColor = '#9ca3af';
                    showToast('当前版本已是最新版本', 'info');
                } else if (data.versionStatus === 'newer') {
                    // 当前版本更高
                    serverVersion.style.color = '#f59e0b';
                    serverOTABtn.disabled = true;
                    serverOTABtn.textContent = '已是最新版本';
                    serverOTABtn.style.backgroundColor = '#9ca3af';
                    showToast('当前版本比服务器版本更新', 'info');
                } else if (data.versionStatus === 'needs_update') {
                    // 需要升级
                    serverVersion.style.color = '#ef4444';
                    serverOTABtn.disabled = false;
                    serverOTABtn.textContent = '开始服务器升级';
                    serverOTABtn.style.backgroundColor = '#10b981';
                    showToast('发现新版本: ' + data.serverVersion.version + '，可以升级', 'success');
                } else {
                    // 未知状态
                    serverVersion.style.color = '#6b7280';
                    serverOTABtn.disabled = true;
                    serverOTABtn.textContent = '版本状态未知';
                    serverOTABtn.style.backgroundColor = '#9ca3af';
                    showToast('无法确定版本状态', 'warning');
                }

                // 添加版本状态提示信息
                if (data.versionMessage) {
                    const versionStatusDiv = document.createElement('div');
                    versionStatusDiv.className = 'version-status-message';
                    versionStatusDiv.style.cssText = 'margin-top: 10px; padding: 8px; border-radius: 4px; font-size: 14px;';

                    if (data.versionStatus === 'needs_update') {
                        versionStatusDiv.style.backgroundColor = '#fef3c7';
                        versionStatusDiv.style.color = '#92400e';
                        versionStatusDiv.style.border = '1px solid #fbbf24';
                    } else {
                        versionStatusDiv.style.backgroundColor = '#f3f4f6';
                        versionStatusDiv.style.color = '#374151';
                        versionStatusDiv.style.border = '1px solid #d1d5db';
                    }

                    versionStatusDiv.textContent = data.versionMessage;

                    // 移除之前的状态消息
                    const existingStatus = versionInfo.querySelector('.version-status-message');
                    if (existingStatus) {
                        existingStatus.remove();
                    }

                    versionInfo.appendChild(versionStatusDiv);
                }
            } else {
                serverVersion.textContent = '获取失败';
                serverVersion.style.color = '#ef4444';
                serverOTABtn.disabled = true;
                serverOTABtn.textContent = '版本获取失败';
                serverOTABtn.style.backgroundColor = '#9ca3af';
            }

            showToast('版本信息获取成功', 'success');
        } else {
            showToast('版本信息获取失败: ' + data.message, 'error');
            versionInfo.classList.remove('hidden');
            serverVersion.textContent = '获取失败';
            serverVersion.style.color = '#ef4444';
        }
    } catch (error) {
        showToast('网络错误: ' + error.message, 'error');
        versionInfo.classList.remove('hidden');
        serverVersion.textContent = '网络错误';
        serverVersion.style.color = '#ef4444';
    }
    finally {
        checkVersionBtn.disabled = false;
        checkVersionBtn.textContent = '检查版本';
    }
}

async function startServerOTA() {
    const serverOTABtn = document.getElementById('serverOTABtn');

    if (!confirm('确定要开始服务器OTA升级吗？\\n\\n注意：升级过程中请勿断开电源或WiFi连接。')) {
        return;
    }

    serverOTABtn.disabled = true;
    serverOTABtn.textContent = '启动中...';

    try {
        const formData = new FormData();
        formData.append('serverUrl', 'http://ota.dlcv.com.cn');
        formData.append('firmwareFile', 'firmware.bin');

        const response = await fetch('/api/ota/server-start', {
            method: 'POST',
            body: formData
        });

        const data = await response.json();

        if (data.success) {
            showToast('服务器OTA升级已启动', 'success');

            document.getElementById('otaProgress').classList.remove('hidden');
            document.getElementById('otaStatusTitle').textContent = '服务器OTA升级进行中...';

            otaStatusInterval = setInterval(updateOTAStatus, 1000);

        } else {
            showToast('服务器OTA升级启动失败: ' + data.message, 'error');
            serverOTABtn.disabled = false;
            serverOTABtn.textContent = '开始服务器升级';
        }
    } catch (error) {
        showToast('网络错误: ' + error.message, 'error');
        serverOTABtn.disabled = false;
        serverOTABtn.textContent = '开始服务器升级';
    }
}
)JS";

const char OTA_PAGE_HTML_HEAD[] = R"HTML(
<!DOCTYPE html>
<html lang=\"zh-CN\">
<head>
<meta charset=\"UTF-8\">
<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">
<title>固件升级 - ESP32S3 Monitor</title>
<style>
)HTML";

const char OTA_PAGE_HTML_BODY_START[] = R"HTML(
</style>
</head>
<body>
<div class=\"container\">
<header class=\"header\">
<h1>小屏幕配置</h1>
<div class=\"subtitle\">固件升级</div>
</header>
<div class=\"card\">
<button onclick=\"window.location.href='/'\" class=\"back-home-btn\">返回首页</button>
<div class=\"ota-section\">
<div class=\"ota-info\">
<p class=\"info-text\">选择升级方式</p>
<p class=\"warning-text\">升级过程中请勿断电或关闭页面</p>
</div>
<div class=\"ota-tabs\">
<button class=\"tab-btn active\" data-tab=\"local\">本地文件</button>
<button class=\"tab-btn\" data-tab=\"server\">服务器升级</button>
</div>
<div class=\"tab-content active\" id=\"localTab\">
<div class=\"file-upload-section\">
<input type=\"file\" id=\"firmwareFile\" accept=\".bin\" style=\"display: none;\">
<button id=\"selectFileBtn\" class=\"file-select-btn\">选择固件文件</button>
<div id=\"fileInfo\" class=\"file-info hidden\">
<p><strong>文件名:</strong> <span id=\"fileName\"></span></p>
<p><strong>文件大小:</strong> <span id=\"fileSize\"></span></p>
</div>
</div>
<div class=\"upload-section\">
<button id=\"uploadBtn\" class=\"upload-btn\" disabled>
<span class=\"btn-text\">开始升级</span>
<div class=\"btn-loading hidden\"><div class=\"spinner-sm\"></div></div>
</button>
</div>
</div>
<div class=\"tab-content\" id=\"serverTab\">
<div class=\"server-ota-section\">
<div class=\"server-info\">
<p class=\"server-text\">从服务器下载最新固件</p>
</div>
<div class=\"server-actions\">
<button id=\"checkVersionBtn\" class=\"server-btn version-btn\">检查版本</button>
<button id=\"serverOTABtn\" class=\"server-btn ota-btn\">开始服务器升级</button>
</div>
<div id=\"versionInfo\" class=\"version-info hidden\">
<h4>版本信息</h4>
<p><strong>当前版本:</strong> <span id=\"currentVersion\">1.0.0</span></p>
<p><strong>服务器版本:</strong> <span id=\"serverVersion\">检查中...</span></p>
</div>
</div>
</div>
<div id=\"otaProgress\" class=\"ota-progress hidden\">
<div class=\"progress-info\">
<h3 id=\"otaStatusTitle\">准备升级...</h3>
<p id=\"otaStatusDetail\"></p>
</div>
<div class=\"progress-bar\">
<div id=\"progressFill\" class=\"progress-fill\"></div>
</div>
<div class=\"progress-text\">
<span id=\"progressPercent\">0%</span>
<span id=\"progressSize\">0 / 0 字节</span>
</div>
</div>
<div id=\"otaResult\" class=\"ota-result hidden\">
<div class=\"result-success hidden\" id=\"successResult\">
<h3>升级成功！</h3>
<p>固件已成功升级，设备将自动重启并刷新页面。</p>
</div>
<div class=\"result-error hidden\" id=\"errorResult\">
<h3>升级失败</h3>
<p id=\"errorMessage\">升级过程中发生错误</p>
<button id=\"retryBtn\" class=\"retry-btn\">重新尝试</button>
</div>
</div>
</div>
</div>
</div>
<div id=\"toast\" class=\"toast hidden\">
<div class=\"toast-content\">
<span id=\"toastMessage\"></span>
</div>
</div>
<script>
)HTML";

const char OTA_PAGE_HTML_BODY_END[] = R"HTML(
</script>
</body>
</html>
)HTML";

// getCSS/getOTAPageCSS/getOTAJavaScript/getOTAPageHTML 改为直接返回常量
std::string getOTAPageHTML() {
    std::string html;
    html += OTA_PAGE_HTML_HEAD;
    html += OTA_CSS;
    html += OTA_PAGE_CSS;
    html += OTA_PAGE_HTML_BODY_START;
    html += OTA_JS;
    html += OTA_PAGE_HTML_BODY_END;
    return html;
}

