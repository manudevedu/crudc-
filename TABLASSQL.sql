CREATE DATABASE universidad


CREATE TABLE alumnos (
    id_alumnos SERIAL PRIMARY KEY,
    nombres VARCHAR(50) NOT NULL,
    apellidos VARCHAR(50) NOT NULL,
    carnet VARCHAR(20) UNIQUE NOT NULL
);

CREATE TABLE cursos (
    id_cursos SERIAL PRIMARY KEY,
    nombre VARCHAR(50) NOT NULL,
    codigo VARCHAR(20) UNIQUE NOT NULL
);

CREATE TABLE secciones (
    id_secciones SERIAL PRIMARY KEY,
    nombre VARCHAR(50) NOT NULL,
    jornada VARCHAR(20) NOT NULL
);

CREATE TABLE alumnos_cursos (
    id_alumnos_cursos SERIAL PRIMARY KEY,
    alumno_id INT NOT NULL,
    curso_id INT NOT NULL,
    CONSTRAINT fk_alumno FOREIGN KEY (alumno_id) REFERENCES alumnos(id_alumnos),
    CONSTRAINT fk_curso FOREIGN KEY (curso_id) REFERENCES cursos(id_cursos)
);

CREATE TABLE alumnos_seccion (
    id_alumnos_seccion SERIAL PRIMARY KEY,
    alumno_id INT NOT NULL,
    seccion_id INT NOT NULL,
    CONSTRAINT fk_alumno_seccion FOREIGN KEY (alumno_id) REFERENCES alumnos(id_alumnos),
    CONSTRAINT fk_seccion FOREIGN KEY (seccion_id) REFERENCES secciones(id_secciones)
);
