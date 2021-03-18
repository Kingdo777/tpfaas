curl:
	echo "{\"value\":{\"name\":\"Kingdo\"}}" > test.json
	curl -d "@test.json" http://127.0.0.1:8080/init
	curl -d "@test.json" http://127.0.0.1:8080/run
	rm test.json

runDocker:buildDocker
	-docker stop tpfaas
	docker run --rm --name tpfaas -p 8080:8080 -d kingdo/tpfaas

buildDocker:build
	docker build -t  kingdo/tpfaas .

.PHONY:build
build:
	mkdir -p build
	cd build && cmake .. && make tpfaas

