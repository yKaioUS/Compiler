from flask import Flask, render_template, request, jsonify
from flask_cors import CORS
import os
import subprocess

app = Flask(__name__, 
           template_folder='../frontend/templates',
           static_folder='../frontend/static')
CORS(app)

UPLOAD_FOLDER = 'temp/uploads'
OUTPUT_FOLDER = 'temp/outputs'
COMPILER_PATH = '../cpp_src/TranslationSchema.exe'

os.makedirs(UPLOAD_FOLDER, exist_ok=True)
os.makedirs(OUTPUT_FOLDER, exist_ok=True)

def parse_output(output_text):    
    lines = output_text.split('\n')
    sections = {
        'lexical': ''
    }
    
    for line in lines:
        sections['lexical'] += line + '\n'
    
    return sections

@app.route('/')
def index():
    return render_template('translationschema.html')

@app.route('/compile', methods=['POST'])
def compile_code():
    if 'file' not in request.files:
        return jsonify({'error': '没有上传文件'}), 400
    
    file = request.files['file']
    
    if file.filename == '':
        return jsonify({'error': '没有选择文件'}), 400
    
    # 生成唯一ID用于本次编译
    file_name = os.path.splitext(file.filename)[0]
    input_path = os.path.join(UPLOAD_FOLDER, f'{file_name}_input.txt')
    output_path = os.path.join(OUTPUT_FOLDER, f'{file_name}_output.txt')
    
    try:
        # 保存上传的文件
        file.save(input_path)
        
        cmd = [COMPILER_PATH, input_path, output_path]
        result = subprocess.run(cmd, capture_output=True, text=True)
        
        if result.returncode != 0:
            if os.path.exists(output_path):
                with open(output_path, 'r', encoding='utf-8') as f:
                    output_text = f.read()
            else:
                output_text = result.stderr
            sections = parse_output(output_text)
            sections['error'] = result.stderr
        else:
            with open(output_path, 'r', encoding='utf-8') as f:
                output_text = f.read()
            sections = parse_output(output_text)
        
        return jsonify(sections)
        
    except Exception as e:
        
        return jsonify({'error': str(e)}), 500

@app.route('/example', methods=['GET'])
def get_example():
    """获取示例代码"""
    example_code = """{
        int a = 1 ; int b = 2 ; real c = 3.0 ;
        {

        a = a + 1 ;

        b = b * a ;

        if ( a < b ) then c = c / 2 ; else c = c / 4 ;

        }
}"""
    return jsonify({'code': example_code})

if __name__ == '__main__':
    if not os.path.exists(COMPILER_PATH):
        print("警告: C++编译器未编译，请先进行编译生成TranslationSchema.exe文件")
    
    app.run(debug=True, port=5000)