DROP SCHEMA IF EXISTS yaChallenge CASCADE;

CREATE SCHEMA IF NOT EXISTS yaChallenge;
CREATE EXTENSION IF NOT EXISTS "uuid-ossp";

-- 1. User table
CREATE TABLE IF NOT EXISTS yaChallenge.users (
    id TEXT PRIMARY KEY DEFAULT uuid_generate_v4(),
    email VARCHAR(255) UNIQUE NOT NULL,
    nickname VARCHAR(100) UNIQUE NOT NULL,
    androidToken TEXT,
    password TEXT NOT NULL,
    categories TEXT[] DEFAULT ARRAY[]::TEXT[]
);

-- 2. Category table
CREATE TABLE IF NOT EXISTS yaChallenge.categories (
    id TEXT PRIMARY KEY DEFAULT uuid_generate_v4();
    imageUrl TEXT,
    title VARCHAR(255) UNIQUE NOT NULL
);

-- 3. Challenge table
CREATE TABLE IF NOT EXISTS yaChallenge.challenges (
    id TEXT PRIMARY KEY DEFAULT uuid_generate_v4(),
    title VARCHAR(255) NOT NULL, 
    imageUrl TEXT,
    description TEXT NOT NULL, 
    category TEXT REFERENCES yaChallenge.categories (id) ON DELETE SET NULL,
    score INT NOT NULL
);

-- 4. CompletedChallenges table
CREATE TABLE IF NOT EXISTS yaChallenge.completedChallenges (
    userId TEXT NOT NULL,
    challengeId TEXT NOT NULL,
    score INT NOT NULL,
    completedAt TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (userId, challengeId),
    FOREIGN KEY (userId) REFERENCES yaChallenge.users (id) ON DELETE CASCADE,
    FOREIGN KEY (challengeId) REFERENCES yaChallenge.challenges (id) ON DELETE CASCADE
);

