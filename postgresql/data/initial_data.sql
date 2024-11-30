INSERT INTO yaChallenge.category (title)
VALUES
    ('Sport'),
    ('Music'),
    ('Technology'),
    ('Art'),
    ('Science')
ON CONFLICT (title)
DO NOTHING;
