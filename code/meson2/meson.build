project('openssl_example', 'cpp', version: '0.1.0')

# Find OpenSSL dependency
openssl_dep = dependency('openssl', required: true)

# Add executable
executable('myprog', 'main.cpp', dependencies: openssl_dep)