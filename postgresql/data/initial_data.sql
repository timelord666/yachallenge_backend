-- 1. Insert Users (password - password)
INSERT INTO yaChallenge.users (email, nickname, androidToken, password) VALUES
('john.doe@example.com', 'JohnDoe', 'androidTokenJohn', '5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8'),
('jane.smith@example.com', 'JaneSmith', 'androidTokenJane', '5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8'),
('bob.brown@example.com', 'BobBrown', 'androidTokenBob', '5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8')
ON CONFLICT DO NOTHING;

-- 2. Insert Categories
INSERT INTO yaChallenge.categories (imageUrl, title) VALUES
('http://example.com/img/sport.png', 'Sport'),
('http://example.com/img/music.png', 'Music'),
('http://example.com/img/technology.png', 'Technology'),
('http://example.com/img/art.png', 'Art'),
('http://example.com/img/science.png', 'Science')
ON CONFLICT DO NOTHING;

-- 3. Insert Challenges
-- Assigning challenges to the new categories
INSERT INTO yaChallenge.challenges (title, imageUrl, description, category, score)
SELECT 'Run 5km',
       'http://example.com/img/run5km.png',
       'Run a total of 5 kilometers in a single session',
       (SELECT id FROM yaChallenge.categories WHERE title='Sport'),
       50
ON CONFLICT DO NOTHING;

INSERT INTO yaChallenge.challenges (title, imageUrl, description, category, score)
SELECT 'Meditate 10 minutes',
       'http://example.com/img/meditate.png',
       'Spend 10 minutes practicing mindfulness',
       (SELECT id FROM yaChallenge.categories WHERE title='Art'),
       30
ON CONFLICT DO NOTHING;

INSERT INTO yaChallenge.challenges (title, imageUrl, description, category, score)
SELECT 'Save $100',
       'http://example.com/img/save100.png',
       'Put aside $100 into your savings account',
       (SELECT id FROM yaChallenge.categories WHERE title='Technology'),
       40
ON CONFLICT DO NOTHING;

INSERT INTO yaChallenge.challenges (title, imageUrl, description, category, score)
SELECT 'Do 30 push-ups',
       'http://example.com/img/pushups.png',
       'Complete 30 push-ups in one go',
       (SELECT id FROM yaChallenge.categories WHERE title='Sport'),
       60
ON CONFLICT DO NOTHING;

INSERT INTO yaChallenge.challenges (title, imageUrl, description, category, score)
SELECT 'Drink 2L of water',
       'http://example.com/img/water.png',
       'Consume at least 2 liters of water in a day',
       (SELECT id FROM yaChallenge.categories WHERE title='Science'),
       20
ON CONFLICT DO NOTHING;

-- 4. Insert User-Categories (Many-to-Many)
-- JohnDoe: Sport, Technology
INSERT INTO yaChallenge.userCategories (userId, categoryId)
SELECT u.id, c.id
FROM yaChallenge.users u, yaChallenge.categories c
WHERE u.nickname='JohnDoe' AND c.title='Sport'
ON CONFLICT DO NOTHING;

INSERT INTO yaChallenge.userCategories (userId, categoryId)
SELECT u.id, c.id
FROM yaChallenge.users u, yaChallenge.categories c
WHERE u.nickname='JohnDoe' AND c.title='Technology'
ON CONFLICT DO NOTHING;

-- JaneSmith: Science, Sport
INSERT INTO yaChallenge.userCategories (userId, categoryId)
SELECT u.id, c.id
FROM yaChallenge.users u, yaChallenge.categories c
WHERE u.nickname='JaneSmith' AND c.title='Science'
ON CONFLICT DO NOTHING;

INSERT INTO yaChallenge.userCategories (userId, categoryId)
SELECT u.id, c.id
FROM yaChallenge.users u, yaChallenge.categories c
WHERE u.nickname='JaneSmith' AND c.title='Sport'
ON CONFLICT DO NOTHING;

-- BobBrown: Technology
INSERT INTO yaChallenge.userCategories (userId, categoryId)
SELECT u.id, c.id
FROM yaChallenge.users u, yaChallenge.categories c
WHERE u.nickname='BobBrown' AND c.title='Technology'
ON CONFLICT DO NOTHING;

-- 5. Insert Completed Challenges
-- JohnDoe completed "Run 5km" (Sport) and "Save $100" (Technology)
INSERT INTO yaChallenge.completedChallenges (userId, challengeId)
SELECT u.id, ch.id
FROM yaChallenge.users u, yaChallenge.challenges ch
WHERE u.nickname='JohnDoe' AND ch.title='Run 5km'
ON CONFLICT DO NOTHING;

INSERT INTO yaChallenge.completedChallenges (userId, challengeId)
SELECT u.id, ch.id
FROM yaChallenge.users u, yaChallenge.challenges ch
WHERE u.nickname='JohnDoe' AND ch.title='Save $100'
ON CONFLICT DO NOTHING;

-- JaneSmith completed "Meditate 10 minutes" (Art)
INSERT INTO yaChallenge.completedChallenges (userId, challengeId)
SELECT u.id, ch.id
FROM yaChallenge.users u, yaChallenge.challenges ch
WHERE u.nickname='JaneSmith' AND ch.title='Meditate 10 minutes'
ON CONFLICT DO NOTHING;

-- BobBrown hasn't completed any challenges yet
