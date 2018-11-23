docker stop chibi_docs; \
docker rm chibi_docs; \
docker run -d -it --restart unless-stopped \
        --name chibi_docs \
        -p 3000:80 \
        chibi_docs:latest
