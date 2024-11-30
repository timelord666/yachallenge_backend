INSERT INTO YaChallenge.Category (Title)
VALUES
    ('Sport'),
    ('Music'),
    ('Technology'),
    ('Art'),
    ('Science')
ON CONFLICT (Title)
DO NOTHING;
