DROP SCHEMA IF EXISTS yaChallenge CASCADE;

CREATE SCHEMA IF NOT EXISTS yaChallenge;
CREATE EXTENSION IF NOT EXISTS "uuid-ossp";

-- 1. User table
CREATE TABLE IF NOT EXISTS yaChallenge.user (
    id TEXT PRIMARY KEY DEFAULT uuid_generate_v4(),
    email VARCHAR(255) UNIQUE NOT NULL,
    nickname VARCHAR(100) UNIQUE NOT NULL,
    androidToken TEXT,
    password TEXT NOT NULL,
    categories UUID[] DEFAULT ARRAY[]::UUID[]
);

-- 2. Category table
CREATE TABLE IF NOT EXISTS yaChallenge.category (
    id TEXT PRIMARY KEY DEFAULT uuid_generate_v4(),
    imageUrl TEXT,
    title VARCHAR(255) UNIQUE NOT NULL
);

-- 3. Challenge table
CREATE TABLE IF NOT EXISTS yaChallenge.challenge (
    id TEXT PRIMARY KEY DEFAULT uuid_generate_v4(),
    title VARCHAR(255) NOT NULL, 
    imageUrl TEXT,
    description TEXT NOT NULL, 
    category UUID REFERENCES yaChallenge.category (id) ON DELETE SET NULL,
    score INT NOT NULL
);

-- 4. CompletedChallenges table
CREATE TABLE IF NOT EXISTS yaChallenge.completedChallenges (
    userId TEXT NOT NULL,
    challengeId UUID NOT NULL,
    score INT NOT NULL,
    completedAt TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (userId, challengeId),
    FOREIGN KEY (userId) REFERENCES yaChallenge.user (id) ON DELETE CASCADE,
    FOREIGN KEY (challengeId) REFERENCES yaChallenge.challenge (id) ON DELETE CASCADE
);

