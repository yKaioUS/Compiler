// frontend/static/js/main.js
document.addEventListener('DOMContentLoaded', function() {
    // 元素引用
    const fileInput = document.getElementById('fileInput');
    const uploadForm = document.getElementById('uploadForm');
    const codePreview = document.getElementById('codePreview');
    const exampleBtn = document.getElementById('exampleBtn');
    const tabs = document.querySelectorAll('.tab');
    const resultSections = document.querySelectorAll('.result-section');
    const statusBar = document.getElementById('statusBar');
    const statusText = statusBar.querySelector('span');

    // 文件选择事件
    fileInput.addEventListener('change', function(e) {
        const file = e.target.files[0];
        if (file) {
            const reader = new FileReader();
            reader.onload = function(e) {
                codePreview.textContent = e.target.result;
                codePreview.classList.remove('error');
            };
            reader.onerror = function() {
                codePreview.textContent = '读取文件失败';
                codePreview.classList.add('error');
            };
            reader.readAsText(file);
        }
    });

    // 表单提交事件
    uploadForm.addEventListener('submit', async function(e) {
        e.preventDefault();
        
        const file = fileInput.files[0];
        if (!file) {
            showStatus('请选择文件', 'error');
            return;
        }

        const formData = new FormData();
        formData.append('file', file);

        showStatus('正在分析...', 'loading');

        try {
            const response = await fetch('/compile', {
                method: 'POST',
                body: formData
            });

            const result = await response.json();

            if (response.ok) {
                // 更新各个结果区域
                updateResult('lexical-result', result.lexical || '无结果');
                
                showStatus('分析完成', 'success');
            } else {
                showStatus(`错误: ${result.error || '未知错误'}`, 'error');
                console.error(result);
            }
        } catch (error) {
            showStatus(`网络错误: ${error.message}`, 'error');
            console.error(error);
        }
    });

    // 示例代码按钮
    exampleBtn.addEventListener('click', async function() {
        try {
            showStatus('加载示例代码...', 'loading');
            const response = await fetch('/example');
            const result = await response.json();
            
            if (response.ok) {
                codePreview.textContent = result.code;
                showStatus('示例代码已加载', 'success');
            }
        } catch (error) {
            showStatus(`加载失败: ${error.message}`, 'error');
        }
    });

    // 标签页切换
    tabs.forEach(tab => {
        tab.addEventListener('click', function() {
            const tabId = this.dataset.tab;
            
            // 更新标签页状态
            tabs.forEach(t => t.classList.remove('active'));
            this.classList.add('active');
            
            // 显示对应内容
            resultSections.forEach(section => {
                section.classList.remove('active');
                if (section.id === `${tabId}-result`) {
                    section.classList.add('active');
                }
            });
        });
    });

    // 辅助函数
    function updateResult(sectionId, content) {
        const section = document.getElementById(sectionId);
        if (section) {
            const contentEl = section.querySelector('.result-content');
            if (contentEl) {
                contentEl.textContent = content;
                
                // 简单的语法高亮（可根据需要扩展）
                if (content.includes('错误') || content.includes('Error')) {
                    contentEl.style.color = '#e06c75';
                } else {
                    contentEl.style.color = '#abb2bf';
                }
            }
        }
    }

    function showStatus(message, type) {
        statusText.textContent = message;
        
        // 根据类型设置图标和颜色
        const icon = statusBar.querySelector('i');
        switch(type) {
            case 'loading':
                icon.className = 'fas fa-spinner fa-spin';
                statusBar.style.color = '#2196F3';
                break;
            case 'success':
                icon.className = 'fas fa-check-circle';
                statusBar.style.color = '#4CAF50';
                break;
            case 'error':
                icon.className = 'fas fa-exclamation-circle';
                statusBar.style.color = '#f44336';
                break;
            default:
                icon.className = 'fas fa-info-circle';
                statusBar.style.color = '#666';
        }
    }

    // 初始化状态
    showStatus('准备就绪', 'info');
});