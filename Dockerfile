FROM gcc:latest
    LABEL oster=oleg.ternikov@oster-industries.ru
    ADD ./src /Mashroom
    # RUN git clone https://github.com/oleg1996323/Mashroom.git
    # RUN cmake --build .
    EXPOSE 8000