INSERT INTO yaChallenge.categories (title)
VALUES
    ('Sport'),
    ('Music'),
    ('Technology'),
    ('Art'),
    ('Science')
ON CONFLICT (title)
DO NOTHING;
