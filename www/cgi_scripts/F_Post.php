<?php

$databaseFile = 'idk.db';
$pdo = new PDO('sqlite:' . $databaseFile);
$pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

$createTableQuery = "
    CREATE TABLE IF NOT EXISTS idk (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        task TEXT NOT NULL
    )
";
$pdo->exec($createTableQuery);

if ($_SERVER["REQUEST_METHOD"] == "POST" && isset($_POST["task"])) {
    $task = $_POST["task"];
    $insertQuery = "INSERT INTO idk (task) VALUES (:task)";
    $stmt = $pdo->prepare($insertQuery);
    $stmt->bindParam(':task', $task, PDO::PARAM_STR);
    $stmt->execute();
}

if (isset($_GET["delete"]) && is_numeric($_GET["delete"])) {
    $deleteId = $_GET["delete"];
    $deleteQuery = "DELETE FROM idk WHERE id = :id";
    $stmt = $pdo->prepare($deleteQuery);
    $stmt->bindParam(':id', $deleteId, PDO::PARAM_INT);
    $stmt->execute();
}

$selectQuery = "SELECT * FROM idk";
$idk = $pdo->query($selectQuery)->fetchAll(PDO::FETCH_ASSOC);
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Simple IDK App</title>
</head>
<body>

<h2>IDK App</h2>

<form method="post" action="/cgi/form_post.php">
    <label for="task">New Task:</label>
    <input type="text" name="task" required>
    <button type="submit">Add Task</button>
</form>

<ul>
    <?php foreach ($idk as $idk): ?>
        <li>
            <?php echo htmlspecialchars($idk["task"]); ?>
            <a href="?delete=<?php echo $idk["id"]; ?>" onclick="return confirm('Are you sure?')">Delete</a>
        </li>
    <?php endforeach; ?>
</ul>

</body>
</html>
