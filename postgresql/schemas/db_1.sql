DROP SCHEMA IF EXISTS YaChallenge CASCADE;

CREATE SCHEMA IF NOT EXISTS YaChallenge;
CREATE EXTENSION IF NOT EXISTS "uuid-ossp";

-- 1. User table
CREATE TABLE IF NOT EXISTS YaChallenge.User (
    UUID UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    Email VARCHAR(255) UNIQUE NOT NULL,
    Nickname VARCHAR(100) UNIQUE NOT NULL,
    AndroidToken TEXT NOT NULL,
    Password TEXT NOT NULL,
    Categories UUID[] DEFAULT ARRAY[]::UUID[]
);

-- 2. Category table
CREATE TABLE IF NOT EXISTS YaChallenge.Category (
    UUID UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    Title VARCHAR(255) UNIQUE NOT NULL
);

-- 3. Challenge table
CREATE TABLE IF NOT EXISTS YaChallenge.Challenge (
    UUID UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    Title VARCHAR(255) NOT NULL, 
    Description TEXT NOT NULL, 
    Category UUID REFERENCES YaChallenge.Category (UUID) ON DELETE SET NULL,
    Score INT NOT NULL
);

-- 4. CompletedChallenges table
CREATE TABLE IF NOT EXISTS YaChallenge.CompletedChallenges (
    UserUUID UUID NOT NULL,
    ChallengeUUID UUID NOT NULL,
    Score INT NOT NULL,
    Completed_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (UserUUID, ChallengeUUID),
    FOREIGN KEY (UserUUID) REFERENCES YaChallenge.User (UUID) ON DELETE CASCADE,
    FOREIGN KEY (ChallengeUUID) REFERENCES YaChallenge.Challenge (UUID) ON DELETE CASCADE
);

