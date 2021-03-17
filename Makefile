curl:
	echo "{\"name\":\"kingdo\"}" > test.json
	curl -d "@test.json" http://127.0.0.1/cmd
	rm test.json

run:distDocker
	docker stop tpfaas
	docker run --rm --name tpfaas -p 80:80 -d kingdo/tpfaas

distDocker:build/tpfaas
	docker build -t  kingdo/tpfaas .

build:
	cd build && cmake .. && make tpfaas

